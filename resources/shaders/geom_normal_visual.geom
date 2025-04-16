#version 330 core

uniform mat4 u_projection;

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 vs_normal[];

const float MAGNITUDE = .2;

void generateLine(int index)
{
    gl_Position = u_projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = u_projection * (gl_in[index].gl_Position + vec4(vs_normal[index], 0.) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main() {
    generateLine(0);
    generateLine(1);
    generateLine(2);
}