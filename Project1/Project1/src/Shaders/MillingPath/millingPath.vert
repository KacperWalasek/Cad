#version 410

layout(location = 0) in vec3 aPos;  
layout(location = 1) in vec2 aUV;   
 
out vec2 uv;
out vec3 pos;

void main()
{
    uv = aUV;
    pos = aPos;
    gl_Position = vec4(aPos,1.0f);
}