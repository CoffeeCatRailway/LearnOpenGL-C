#version 330 core
precision mediump float;

#define F_LHT_DIRECT 1
#define F_LHT_POINT 2
#define F_LHT_SPOT 3

#define N_LIGHTS 8

struct Material
{
	sampler2D diffuseTex;
	sampler2D specularTex;
	
	float shininess;
};

struct Light
{
	int mode;
	vec3 position;
	vec3 direction;
	float cutOffInner;
	float cutOffOuter;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform vec3 u_viewPos;
uniform Material u_material;

uniform Light u_lights[N_LIGHTS];

in vec3 v_fragPos;
in vec3 v_normal;
in vec2 v_uv;

out vec4 FragColor;

vec4 calcDirectLight(Light light, vec3 normal, vec3 viewDir, vec4 matDiffuse, vec4 matSpecular);
vec4 calcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 matDiffuse, vec4 matSpecular);
vec4 calcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 matDiffuse, vec4 matSpecular);

float near = .1;
float far = 100.;

float linearizeDepth(float depth)
{
	float z = depth * 2. - 1.;
	return (2. * near * far) / (far + near - z * (far - near));
}

void main()
{
	// pre-calculate material texture values
	vec4 matDiffuse = texture(u_material.diffuseTex, v_uv);
	if (matDiffuse.a < .1)
	discard;
	vec4 matSpecular = texture(u_material.specularTex, v_uv);

	// global properties
	vec3 norm = normalize(v_normal);
	vec3 viewDir = normalize(u_viewPos - v_fragPos);
	
	// calculate lights
	vec4 result = vec4(0.);
	for (int i = 0; i < N_LIGHTS; i++)
	{
		Light light = u_lights[i];
		if (light.mode == F_LHT_DIRECT)
			result += calcDirectLight(light, norm, viewDir, matDiffuse, matSpecular);
		
		if (light.mode == F_LHT_POINT)
			result += calcPointLight(light, norm, v_fragPos, viewDir, matDiffuse, matSpecular);
		
		if (light.mode == F_LHT_SPOT)
			result += calcSpotLight(light, norm, v_fragPos, viewDir, matDiffuse, matSpecular);
	}

//	float depth = linearizeDepth(gl_FragCoord.z) / far;
//	vec4 depthVec = vec4(pow(depth, 1.4));
//	result.rgb = norm * .5 + .5;
	FragColor = result;// * (1. - depthVec) + depthVec;
}

vec4 calcDirectLight(Light light, vec3 normal, vec3 viewDir, vec4 matDiffuse, vec4 matSpecular)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse
	float diff = max(dot(normal, lightDir), 0.);
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.), u_material.shininess);
	// combine
	vec4 ambient = vec4(light.ambient, 1.) * matDiffuse;
	vec4 diffuse = vec4(light.diffuse, 1.) * diff * matDiffuse;
	vec4 specular = vec4(light.specular, 1.) * spec * matSpecular;
	return ambient + diffuse + specular;
}

vec4 calcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 matDiffuse, vec4 matSpecular)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse
	float diff = max(dot(normal, lightDir), 0.);
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.), u_material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1. / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// combine
	vec4 ambient = vec4(light.ambient, 1.) * matDiffuse;
	vec4 diffuse = vec4(light.diffuse, 1.) * diff * matDiffuse;
	vec4 specular = vec4(light.specular, 1.) * spec * matSpecular;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return ambient + diffuse + specular;
}

vec4 calcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 matDiffuse, vec4 matSpecular)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse
	float diff = max(dot(normal, lightDir), 0.);
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.), u_material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1. / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// spotlight intensoty
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOffInner - light.cutOffOuter;
	float intensity = smoothstep(0., 1., (theta - light.cutOffOuter) / epsilon);
	// combine
	vec4 ambient = vec4(light.ambient, 1.) * matDiffuse;
	vec4 diffuse = vec4(light.diffuse, 1.) * diff * matDiffuse;
	vec4 specular = vec4(light.specular, 1.) * spec * matSpecular;
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return ambient + diffuse + specular;
}
