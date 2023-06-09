#version 410 core

layout (isolines) in;

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;

uniform bool reverse;

uniform int divisionU, divisionV;

patch in vec4 gregoryPoints[20];


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
    float t1 = gl_TessCoord.x;
    float d = reverse ? divisionU : divisionV;
    float t2 = gl_TessCoord.y * d / (d-1);
    
    float u = reverse ? t2 : t1;
    float v = reverse ? t1 : t2;

    vec4 bp[16];

    for(int i =0; i<5; i++)
        bp[i] = gregoryPoints[i];
    
    bp[5] = (u * gregoryPoints[5] + v * gregoryPoints[6]) / (u+v);
    bp[6] = ((1-u) * gregoryPoints[7] + v * gregoryPoints[8]) / (1-u+v);

    bp[7] = gregoryPoints[9];
    bp[8] = gregoryPoints[10];
    
    bp[9] = (u * gregoryPoints[11] + (1-v) * gregoryPoints[12]) / (1+u-v);
    bp[10] = ((1-u) * gregoryPoints[13] + (1-v) * gregoryPoints[14]) / (2-u-v);

    for(int i = 11; i<16; i++)
        bp[i] = gregoryPoints[i+4];



    
    vec4 curve[4];
    for(int i = 0; i<4; i++)
        if(reverse)
            curve[i] = deCastiljeu(t1, bp[i], bp[4+i],bp[8+i],bp[12+i]);
        else
            curve[i] = deCastiljeu(t1, bp[i*4], bp[i*4+1],bp[i*4+2],bp[i*4+3]);

    vec4 pos = deCastiljeu(t2, curve[0],curve[1],curve[2],curve[3]);

    mat4 transform = projMtx * viewMtx * modelMtx;
    
    gl_Position = normalize(transform*pos);
}