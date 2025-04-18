#version 330 core

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform int u_isInstance;

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_uv;
layout (location = 3) in mat4 i_instanceMatrix;

out vec3 v_fragPos;
out vec3 v_normal;
out vec2 v_uv;

void main()
{
	mat4 model = mat4(mix(u_model[0], i_instanceMatrix[0], float(u_isInstance)),
		mix(u_model[1], i_instanceMatrix[1], float(u_isInstance)),
		mix(u_model[2], i_instanceMatrix[2], float(u_isInstance)),
		mix(u_model[3], i_instanceMatrix[3], float(u_isInstance))
	);
	v_fragPos = vec3(model * vec4(i_position, 1.));

//	v_normal = normalize(i_normal);
	v_normal = normalize(mat3(transpose(inverse(model))) * i_normal);
//	v_normal = normalize(cross(dFdx(v_fragPos), dFdy(v_fragPos)));
	v_uv = i_uv;
	
	gl_Position = u_projection * u_view * vec4(v_fragPos, 1.);
}