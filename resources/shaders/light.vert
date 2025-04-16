#version 330 core

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

in vec3 i_position;
in vec3 i_normal;
in vec2 i_uv;

out vec3 v_fragPos;
out vec3 v_normal;
out vec2 v_uv;

void main()
{
	v_fragPos = vec3(u_model * vec4(i_position, 1.));
	//v_normal = i_normal;
	v_normal = normalize(mat3(transpose(inverse(u_model))) * i_normal);
//	v_normal = normalize(cross(dFdx(v_fragPos), dFdy(v_fragPos)));
	v_uv = i_uv;
	
	gl_Position = u_projection * u_view * vec4(v_fragPos, 1.);
}