#version 410 core

layout (isolines) in;

patch in vec4 bp0;
patch in vec4 bp1;
patch in vec4 bp2;
patch in vec4 bp3;

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;

uniform float t0;
uniform float t1;

void main()
{
    float t = t0 + gl_TessCoord.x * (t1-t0);
    
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
    
    vec4 p = n[0]*bp0 + n[1]*bp1 + n[2]*bp2 + n[3] * bp3;
    
    mat4 transform = projMtx * viewMtx * modelMtx;
    gl_Position = normalize(transform*vec4(p.xyz, 1.0f));
}