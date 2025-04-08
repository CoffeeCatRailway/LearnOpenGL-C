#version 330 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texCoords;

out vec2 v_texCoords;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main()
{
    v_texCoords = i_texCoords;
    gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.);
}
