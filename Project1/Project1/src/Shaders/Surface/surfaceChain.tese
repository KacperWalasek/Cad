#version 410 core

layout (isolines) in;

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;
uniform bool reverse;

patch in vec4 bp[16];

void main()
{   
    float x = reverse ? gl_TessCoord.y*4 : gl_TessCoord.x*3;
    float y = reverse ? gl_TessCoord.x*3 : gl_TessCoord.y*4;
    vec4 pos = bp[int(round(x * 4+y))];

    mat4 transform = projMtx * viewMtx * modelMtx;
    
    gl_Position = normalize(transform*pos);
}