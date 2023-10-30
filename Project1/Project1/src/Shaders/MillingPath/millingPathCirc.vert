#version 410

layout(location = 0) in vec3 aPos;  
layout(location = 1) in float aDist;   
 
out float dist;
out vec2 textureUV;

void main()
{
    dist = aDist;
    textureUV = aPos.xy;
    gl_Position = vec4(aPos,1.0f);
}