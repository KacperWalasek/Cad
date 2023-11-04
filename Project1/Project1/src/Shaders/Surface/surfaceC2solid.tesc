#version 410 core

layout (vertices=4) out;

uniform int divisionU, divisionV;

patch out vec4 bp[16];


void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    if (gl_InvocationID == 0)
    { 
        for(int i = 0; i<16; i++)
        {
            bp[i] = gl_in[i].gl_Position;
        }
        gl_TessLevelOuter[0] = divisionV;
        gl_TessLevelOuter[1] = divisionU;
        gl_TessLevelOuter[2] = divisionV;
        gl_TessLevelOuter[3] = divisionU;

        gl_TessLevelInner[0] = divisionU;
        gl_TessLevelInner[1] = divisionV;

    }
}