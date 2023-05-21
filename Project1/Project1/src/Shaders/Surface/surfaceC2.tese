#version 410 core

layout (isolines) in;

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;

uniform bool reverse;

uniform int divisionU, divisionV;

patch in vec4 bp[16];


vec4 deBoor(float t, vec4 a, vec4 b, vec4 c, vec4 d)
{
   vec4 n = vec4(0, 0, 0, 1);
    
    for(int i = 0; i<3; i++)
        for(int j = 2-i; j<4; j++)
        {
            int ind = j-2;
            int m = i+1;
            if(j!=2-i)
                n[j] *= (t-(ind-1))/m;
            if(j!=3)
                n[j] = n[j] + n[j+1] * (ind+m-t)/m;         
        }
    
    return n[0]*a + n[1]*b + n[2]*c + n[3] * d;
}

void main()
{   
    float t = gl_TessCoord.x;
    
    vec4 curve[4];
    for(int i = 0; i<4; i++)
        if(reverse)
            curve[i] = deBoor(t, bp[i], bp[4+i],bp[8+i],bp[12+i]);
        else
            curve[i] = deBoor(t, bp[i*4], bp[i*4+1],bp[i*4+2],bp[i*4+3]);

    float d = reverse ? divisionU : divisionV;
    t = gl_TessCoord.y * d / (d-1);

    vec4 pos = deBoor(t, curve[0],curve[1],curve[2],curve[3]);

    mat4 transform = projMtx * viewMtx * modelMtx;
    
    gl_Position = normalize(transform*pos);
}