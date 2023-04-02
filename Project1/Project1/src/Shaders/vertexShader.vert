#version 410

layout(location = 0) in vec3 aPos;   

uniform mat4 transform;

void main()
{
    gl_Position = normalize(transform*vec4(aPos, 1.0));
}