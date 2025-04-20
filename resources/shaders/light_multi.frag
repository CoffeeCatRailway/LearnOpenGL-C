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

vec3 blinnPhong(Light light, vec3 viewDir, vec3 normal, vec3 fragPos, vec3 specularMap);
vec3 phong(Light light, vec3 viewDir, vec3 normal, vec3 fragPos, vec3 diffuseMap, vec3 specularMap);

vec3 calcDirectLight(Light light, vec3 normal, vec3 viewDir, vec3 diffuseMap, vec3 specularMap);
vec3 calcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseMap, vec3 specularMap);
vec3 calcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseMap, vec3 specularMap);

void main()
{
	// pre-calculate material texture values
	vec4 diffuseMap = texture(u_material.diffuseTex, v_uv);
	if (diffuseMap.a < .1)
		discard;
	vec3 specularMap = texture(u_material.specularTex, v_uv).rgb;

	vec3 viewDir = normalize(u_viewPos - v_fragPos);

	bool blinn = true;
	vec3 result = vec3(0.);
	for (int i = 0; i < N_LIGHTS; i++)
	{
		if (!u_lights[i].enable)
			continue;
		if (blinn)
			result += blinnPhong(u_lights[i], viewDir, v_normal, v_fragPos, specularMap);
		else
			result += phong(u_lights[i], viewDir, v_normal, v_fragPos, diffuseMap.rgb, specularMap);
	}
	if (blinn)
	{
		result = diffuseMap.rgb * result;

		result = pow(result, vec3(1. / 2.));
	}

//	vec3 I = normalize(v_fragPos - u_viewPos);
//	vec3 R = refract(I, v_normal, 1. / 1.33);
////	vec3 R = reflect(I, v_normal);
//	vec3 skyColor = texture(u_skybox, R).rgb;
//	result = skyColor;
//	result = mix(result, skyColor, (result.x + result.y + result.z) / 3.);

//	result = v_normal * .5 + .5;
//	result = vec3(v_uv, 0.);

	FragColor = vec4(result, diffuseMap.a);// * (1. - depthVec) + depthVec;
}

vec3 blinnPhong(Light light, vec3 viewDir, vec3 normal, vec3 fragPos, vec3 specularMap)
{
	// ambient
	vec3 ambient = .1 * light.specular;

	// diffuse
	vec3 lightDir;
	if (light.mode == F_LHT_DIRECT)
	lightDir = normalize(-light.direction);
	else
	lightDir = normalize(light.position - fragPos);
	float diff = max(dot(lightDir, normal), 0.);
	vec3 diffuse = diff * light.specular;

	// specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.), u_material.shininess);
	vec3 specular = spec * light.specular * specularMap;

	if (light.mode != F_LHT_DIRECT)
	{
		// attenuation
		float distance = length(light.position - fragPos);
		float attenuation = 1. / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

		// spotlight intensoty
		float intensity = 1.;
		if (light.mode == F_LHT_SPOT)
		{
			float theta = dot(lightDir, normalize(-light.direction));
			float epsilon = light.cutOffInner - light.cutOffOuter;
			intensity = smoothstep(0., 1., (theta - light.cutOffOuter) / epsilon);
		}

		// combine
		ambient *= attenuation * intensity;
		diffuse *= attenuation * intensity;
		specular *= attenuation * intensity;
	}
	return ambient + diffuse + specular;
}

vec3 phong(Light light, vec3 viewDir, vec3 normal, vec3 fragPos, vec3 diffuseMap, vec3 specularMap)
{
	vec3 result = vec3(0.);
	if (light.mode == F_LHT_DIRECT)
	result += calcDirectLight(light, normal, viewDir, diffuseMap, specularMap);

	if (light.mode == F_LHT_POINT)
	result += calcPointLight(light, normal, fragPos, viewDir, diffuseMap, specularMap);

	if (light.mode == F_LHT_SPOT)
	result += calcSpotLight(light, normal, fragPos, viewDir, diffuseMap, specularMap);
	return result;
}

vec3 calcDirectLight(Light light, vec3 normal, vec3 viewDir, vec3 diffuseMap, vec3 specularMap)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse
	float diff = max(dot(normal, lightDir), 0.);
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.), u_material.shininess);
	// combine
	vec3 ambient = light.ambient * diffuseMap;
	vec3 diffuse = light.diffuse * diff * diffuseMap;
	vec3 specular = light.specular * spec * specularMap;
	return ambient + diffuse + specular;
}

vec3 calcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseMap, vec3 specularMap)
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
	vec3 ambient = light.ambient * diffuseMap;
	vec3 diffuse = light.diffuse * diff * diffuseMap;
	vec3 specular = light.specular * spec * specularMap;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return ambient + diffuse + specular;
}

vec3 calcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseMap, vec3 specularMap)
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
	vec3 ambient = light.ambient * diffuseMap;
	vec3 diffuse = light.diffuse * diff * diffuseMap;
	vec3 specular = light.specular * spec * specularMap;
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return ambient + diffuse + specular;
}
