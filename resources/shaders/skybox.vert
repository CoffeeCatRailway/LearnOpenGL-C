#version 330 core

in vec3 i_position;

uniform mat4 u_projection;
uniform mat4 u_view;

out vec3 v_uv;

void main()
{
    v_uv = i_position;
    vec4 pos = u_projection * u_view * vec4(i_position, 1.);
    gl_Position = pos.xyww;
}