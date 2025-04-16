#version 330 core

in vec3 v_uv;

uniform samplerCube u_texture;

out vec4 FragColor;

void main()
{
    FragColor = texture(u_texture, v_uv);
}