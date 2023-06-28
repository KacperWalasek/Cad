#version 410

layout(location = 0) in vec3 vert_pos;   
layout(location = 1) in vec2 vert_uv;   

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;

out vec2 uv;

void main()
{
   mat4 transform = projMtx * viewMtx * modelMtx;
   gl_Position = normalize( transform * vec4(vert_pos, 1.0) );
   uv = vert_uv;
}