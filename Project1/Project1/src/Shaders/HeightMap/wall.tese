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
uniform int index;

vec3 reorderVector(vec3 v) {
        switch (index) {
            case 0:
                return v;
            case 1:
                return vec3(v.z,v.y,v.x);
            case 2:
                return v + vec3(1.0f,0,0);
            case 3:
                return vec3(v.z,v.y,v.x+1.0f);
        }
}

void main()
{
    float u = offsetUBeg + gl_TessCoord.x * (offsetUEnd-offsetUBeg);
    float v = gl_TessCoord.y; // 0 lub 1

    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;
    
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;

    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = v>0.5f ? p0 : p1;
    
    mat4 transform = projMtx * viewMtx * modelMtx;

    vec2 textureUV;
    switch (index) {
        case 0:
            textureUV = vec2(u,0.0f);
            break;
        case 1:
            textureUV = vec2(0.0f,u);
            break;
        case 2:
            textureUV = vec2(u,1.0f);
            break;
        case 3:
            textureUV = vec2(1.0f,u);
            break;
    }
    
    float dz = texture(tex,textureUV).x;
    
    vec3 deformedPosition;
    
    if(v>0.5f) 
        deformedPosition = reorderVector(p.xyz);
    else 
        deformedPosition = reorderVector(vec3(0.0f,dz,p.z));
    gl_Position = normalize(transform*vec4(deformedPosition, 1.0f));
    worldPos = normalize(modelMtx*vec4(deformedPosition, 1.0f));
    worldPos /= worldPos.w;
    
    lightVec = normalize(lightPos - worldPos.xyz);
    viewVec = normalize(cameraPos - worldPos.xyz);
}