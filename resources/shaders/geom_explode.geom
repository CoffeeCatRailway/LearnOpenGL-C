#version 330 core

uniform float u_time;

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 vs_uv[];
out vec2 ge_uv;

vec4 explode(vec4 position, vec3 normal)
{
    float mag = 2.;
    vec3 direction = normal * (sin(u_time * .5) * .5 + .5) * mag;
    return position + vec4(direction, 0.);
}

vec3 getNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main() {
    vec3 normal = getNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    ge_uv = vs_uv[0];
    EmitVertex();

    gl_Position = explode(gl_in[1].gl_Position, normal);
    ge_uv = vs_uv[1];
    EmitVertex();

    gl_Position = explode(gl_in[2].gl_Position, normal);
    ge_uv = vs_uv[2];
    EmitVertex();
    EndPrimitive();
}