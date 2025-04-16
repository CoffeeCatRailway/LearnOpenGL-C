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
	bool enable;
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

uniform samplerCube u_skybox;

uniform vec3 u_viewPos;
uniform Material u_material;

uniform Light u_lights[N_LIGHTS];

in vec3 v_fragPos;
in vec3 v_normal;
in vec2 v_uv;

out vec4 FragColor;

vec3 calcDirectLight(Light light, vec3 normal, vec3 viewDir, vec3 matDiffuse, vec3 matSpecular);
vec3 calcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 matDiffuse, vec3 matSpecular);
vec3 calcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 matDiffuse, vec3 matSpecular);

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
	vec3 viewDir = normalize(u_viewPos - v_fragPos);

	vec3 I = normalize(v_fragPos - u_viewPos);
	vec3 R = refract(I, v_normal, 1. / 1.33);
//	vec3 R = reflect(I, v_normal);
	vec3 skyColor = texture(u_skybox, R).rgb;
	
	// calculate lights
	vec3 result = vec3(0.);
	for (int i = 0; i < N_LIGHTS; i++)
	{
		Light light = u_lights[i];
		if (!light.enable)
			continue;
		if (light.mode == F_LHT_DIRECT)
			result += calcDirectLight(light, v_normal, viewDir, matDiffuse.rgb, matSpecular.rgb);
		
		if (light.mode == F_LHT_POINT)
			result += calcPointLight(light, v_normal, v_fragPos, viewDir, matDiffuse.rgb, matSpecular.rgb);
		
		if (light.mode == F_LHT_SPOT)
			result += calcSpotLight(light, v_normal, v_fragPos, viewDir, matDiffuse.rgb, matSpecular.rgb);
	}

//	float depth = linearizeDepth(gl_FragCoord.z) / far;
//	vec4 depthVec = vec4(pow(depth, 1.4));
//	result = v_normal * .5 + .5;

//	float average = (result.x + result.y + result.z) / 3.;
//	result = mix(result, skyColor, average);
	result = skyColor;

	FragColor = vec4(result, matDiffuse.a);// * (1. - depthVec) + depthVec;
}

vec3 calcDirectLight(Light light, vec3 normal, vec3 viewDir, vec3 matDiffuse, vec3 matSpecular)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse
	float diff = max(dot(normal, lightDir), 0.);
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.), u_material.shininess);
	// combine
	vec3 ambient = light.ambient * matDiffuse;
	vec3 diffuse = light.diffuse * diff * matDiffuse;
	vec3 specular = light.specular * spec * matSpecular;
	return ambient + diffuse + specular;
}

vec3 calcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 matDiffuse, vec3 matSpecular)
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
	vec3 ambient = light.ambient * matDiffuse;
	vec3 diffuse = light.diffuse * diff * matDiffuse;
	vec3 specular = light.specular * spec * matSpecular;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return ambient + diffuse + specular;
}

vec3 calcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 matDiffuse, vec3 matSpecular)
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
	vec3 ambient = light.ambient * matDiffuse;
	vec3 diffuse = light.diffuse * diff * matDiffuse;
	vec3 specular = light.specular * spec * matSpecular;
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return ambient + diffuse + specular;
}
