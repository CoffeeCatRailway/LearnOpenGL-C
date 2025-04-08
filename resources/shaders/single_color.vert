#version 330 core

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

in vec3 i_position;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.);
}