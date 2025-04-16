#version 330 core

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

in vec3 i_position;
in vec3 i_normal;
in vec2 i_uv;

out vec3 vs_normal;

void main() {
    mat3 normalMatrix = mat3(transpose(inverse(u_view * u_model)));
    vs_normal = vec3(vec4(normalMatrix * i_normal, 0.));
    gl_Position = u_view * u_model * vec4(i_position, 1.);
}