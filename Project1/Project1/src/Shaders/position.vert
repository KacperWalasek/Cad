#version 410

layout(location = 0) in vec3 aPos;   

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;

out vec4 pos;

void main()
{
    mat4 transform = projMtx * viewMtx * modelMtx;
    gl_Position = normalize( transform * vec4(aPos, 1.0) );
    pos = vec4(aPos, 1.0);
}