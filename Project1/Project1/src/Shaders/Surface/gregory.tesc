#version 410 core

layout (vertices=2) out;

uniform int divisionU, divisionV;
uniform bool reverse;

patch out vec4 gregoryPoints[20];


void main()
{
    if(gl_InvocationID == 0) {
            gl_out[gl_InvocationID].gl_Position = gl_in[0].gl_Position;
    }

    if(gl_InvocationID == 1) {
            gl_out[gl_InvocationID].gl_Position = gl_in[3].gl_Position;
    }


    if (gl_InvocationID == 0)
    { 
        for(int i = 0; i<20; i++)
            gregoryPoints[i] = gl_in[i].gl_Position;
        gl_TessLevelOuter[0] = reverse ? divisionU : divisionV;
        gl_TessLevelOuter[1] = (reverse ? divisionV : divisionU)-1;
    }
}