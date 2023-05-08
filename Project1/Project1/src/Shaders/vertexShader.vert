#version 410

layout(location = 0) in vec3 aPos;   

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;

void main()
{
    mat4 transform = projMtx * viewMtx * modelMtx;
    gl_Position = normalize( transform * vec4(aPos, 1.0) );
}