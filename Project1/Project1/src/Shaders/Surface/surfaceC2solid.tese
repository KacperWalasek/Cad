#version 410 core

layout (quads) in;

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;

uniform int divisionU, divisionV;

patch in vec4 bp[16];

out vec4 pos;

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
            curve[i] = deBoor(t, bp[i*4], bp[i*4+1],bp[i*4+2],bp[i*4+3]);

    float d = divisionV;
    t = gl_TessCoord.y * d / (d-1);

    pos = deBoor(t, curve[0],curve[1],curve[2],curve[3]);
    
    pos = modelMtx * pos;
    gl_Position = projMtx *  viewMtx * pos;

}