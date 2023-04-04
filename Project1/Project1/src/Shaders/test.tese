#version 410 core

layout (isolines) in;
patch in vec4 bp0;
patch in vec4 bp1;
patch in vec4 bp2;
patch in vec4 bp3;

uniform mat4 transform;

void main()
{
    float t = gl_TessCoord.x;
    float nt = 1-t;

    vec4 d = bp3;
    vec4 c = bp2;
    vec4 b = bp1;

    d = d * t + c * nt;
    c = c * t + b * nt;
    b = b * t + bp0 * nt;

    d = d * t + c * nt;
    c = c * t + b * nt;

    d = d * t + c * nt;

    vec3 p0 = gl_TessCoord * bp0.xyz;
    vec3 p1 = (1.0f-gl_TessCoord.x) * bp3.xyz;
    vec3 tePosition = p0 + p1;

    gl_Position = normalize(transform*vec4(d.xyz, 1.0f));
}