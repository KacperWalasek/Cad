#version 410

layout(location = 0) in vec3 pos;   
uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;

out vec2 uv;

void main()
{
    mat4 transform = projMtx * viewMtx * modelMtx;
    gl_Position = transform*vec4(pos,1.0f);
    uv = pos.xz;
}