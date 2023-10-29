#version 410

layout(location = 0) in vec3 aPos;  
layout(location = 1) in float aDist;   
 
out float dist;

void main()
{
    dist = aDist;
    gl_Position = vec4(aPos,1.0f);
}