#version 410 core

layout (quads) in;

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;

uniform vec3 lightPos;
uniform vec3 cameraPos;

uniform sampler2D tex;

out vec4 worldPos;
out vec3 lightVec;
out vec3 viewVec;

uniform float offsetUBeg;
uniform float offsetUEnd;
uniform float offsetVBeg;
uniform float offsetVEnd;

void main()
{
    vec2 begin = vec2(offsetUBeg,offsetVBeg);
    vec2 end = vec2(offsetUEnd,offsetVEnd);
    vec2 uv = begin + gl_TessCoord.xy * (end-begin);
    float h =  texture(tex,uv).x;
    
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;
    
    float u = uv.x;
    float v = uv.y;
    
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;

    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;
    
    mat4 transform = projMtx * viewMtx * modelMtx;
    gl_Position = normalize(transform*vec4(p.x, p.y+h, p.z, 1.0f));
    worldPos = normalize(modelMtx*vec4(p.x, p.y+h, p.z, 1.0f));
    worldPos /= worldPos.w;
    
    lightVec = normalize(lightPos - worldPos.xyz);
    viewVec = normalize(cameraPos - worldPos.xyz);
}