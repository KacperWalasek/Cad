#version 410

layout(location = 0) in vec3 pos;   
layout(location = 1) in vec3 norm;   

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;

uniform vec3 lightPos;
uniform vec3 cameraPos;

out vec3 normal;
out vec3 lightVec;
out vec3 viewVec;

void main()
{
    mat4 transform = projMtx * viewMtx * modelMtx;
    gl_Position = transform*vec4(pos,1.0f);
    normal = normalize(norm);
    lightVec = normalize(lightPos - (modelMtx*vec4(pos,1.0f)).xyz);
    viewVec = normalize(cameraPos - (modelMtx*vec4(pos,1.0f)).xyz);
}