#version 330 core

#define F_MAT_DIFFUSE 1 << 0
#define F_MAT_SPECULAR 1 << 1
#define F_MAT_EMISSION 1 << 2

#define F_LHT_DIRECT 1
#define F_LHT_POINT 2
#define F_LHT_SPOT 3

struct Material
{
	int flags;
	sampler2D diffuseTex;
	sampler2D specularTex;
	sampler2D emissionTex;
	
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
uniform Light u_lights[1];

in vec3 v_fragPos;
in vec3 v_normal;
in vec2 v_uv;

out vec4 FragColor;

void main()
{
	vec3 norm = normalize(v_normal);
	vec3 lightDir = vec3(0.);
	
	// ambient & diffuse
	vec3 ambient = vec3(0.);
	vec3 diffuse = vec3(0.);
	if ((u_material.flags & F_MAT_DIFFUSE) != 0)
	{
		// ambient
		ambient = u_lights[0].ambient * texture(u_material.diffuseTex, v_uv).rgb;
		
		// diffuse
		if (u_lights[0].mode == F_LHT_DIRECT)
			lightDir = normalize(-u_lights[0].direction);
		else if (u_lights[0].mode == F_LHT_POINT || u_lights[0].mode == F_LHT_SPOT)
			lightDir = normalize(u_lights[0].position - v_fragPos);
		
		float diff = max(dot(norm, lightDir), 0.);
		diffuse = u_lights[0].diffuse * diff * texture(u_material.diffuseTex, v_uv).rgb;
	}
	
	// specular
	vec3 specular = vec3(0.);
	vec3 specularTex = vec3(0.);
	if ((u_material.flags & F_MAT_SPECULAR) != 0)
	{
		vec3 viewDir = normalize(u_viewPos - v_fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		
		float spec = pow(max(dot(viewDir, reflectDir), 0.), u_material.shininess);
		specularTex = texture(u_material.specularTex, v_uv).rgb;
		specular = u_lights[0].specular * spec * specularTex;
	}
	
	// emission
	vec3 emission = vec3(0.);
	if ((u_material.flags & F_MAT_EMISSION) != 0)
	{
		vec3 emissionMask = vec3(1.);
		if ((u_material.flags & F_MAT_SPECULAR) != 0)
			emissionMask = step(vec3(1.), vec3(1.) - specularTex);
		emission = texture(u_material.emissionTex, v_uv).rgb * emissionMask;
	}
	
	// spot light
	if (u_lights[0].mode == F_LHT_SPOT)
	{
		float theta = dot(lightDir, normalize(-u_lights[0].direction));
		float epsilon = u_lights[0].cutOffInner - u_lights[0].cutOffOuter;
		float intensity = smoothstep(0., 1., (theta - u_lights[0].cutOffOuter) / epsilon);
		diffuse *= intensity;
		specular *= intensity;
	}
	
	// attenuation
	if (u_lights[0].mode == F_LHT_POINT)
	{
		float distance = length(u_lights[0].position - v_fragPos);
		float attenuation = 1. / (u_lights[0].constant + u_lights[0].linear * distance + u_lights[0].quadratic * (distance * distance));
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
	}
	
	vec3 result = ambient + diffuse + specular + emission;
//	result *= sin(u_time) * 1.5 + 2.5;
	FragColor = vec4(result, 1.);
}