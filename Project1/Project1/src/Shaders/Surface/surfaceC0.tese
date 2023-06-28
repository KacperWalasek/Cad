#version 410 core

layout (isolines) in;

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;

uniform bool reverse;

uniform int divisionU, divisionV;

patch in vec4 bp[16];
in vec2 vertUv[];

out vec2 uv;

vec4 deCastiljeu(float t, vec4 a, vec4 b, vec4 c, vec4 d)
{
    float nt = 1-t;

    d = d * t + c * nt;
    c = c * t + b * nt;
    b = b * t + a * nt;

    d = d * t + c * nt;
    c = c * t + b * nt;

    d = d * t + c * nt;

    return d;
}

void main()
{   
    float t = gl_TessCoord.x;
    
    vec4 curve[4];
    for(int i = 0; i<4; i++)
        if(reverse)
            curve[i] = deCastiljeu(t, bp[i], bp[4+i],bp[8+i],bp[12+i]);
        else
            curve[i] = deCastiljeu(t, bp[i*4], bp[i*4+1],bp[i*4+2],bp[i*4+3]);
            
    if(reverse)
        uv.y = t;
    else
        uv.x = t;

    float d = reverse ? divisionU : divisionV;
    t = gl_TessCoord.y * d / (d-1);

    vec4 pos = deCastiljeu(t, curve[0],curve[1],curve[2],curve[3]);

    mat4 transform = projMtx * viewMtx * modelMtx;
    
    gl_Position = normalize(transform*pos);

    if(reverse)
        uv.x = t;
    else
        uv.y = t;

    uv =  (1-uv) * vertUv[0] + uv * vertUv[1];
}