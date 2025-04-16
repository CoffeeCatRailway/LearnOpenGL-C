#version 330 core

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

in vec3 i_position;
in vec3 i_normal;
in vec2 i_uv;

out vec2 vs_uv;

void main() {
    vs_uv = i_uv;
    gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.);
}