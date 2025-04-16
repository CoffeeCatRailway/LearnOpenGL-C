#version 330 core

uniform sampler2D u_texture;

in vec2 ge_uv;

out vec4 FragColor;

void main() {
    FragColor = texture(u_texture, ge_uv);
}