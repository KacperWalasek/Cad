#version 410

layout(location = 0) in vec3 aPos;   

void main()
{
    gl_Position = vec4((aPos * 2.0f) - 1, 1.0);
}