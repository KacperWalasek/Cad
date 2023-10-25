#version 410 core

layout (vertices=4) out;

uniform int divisionU;
uniform int divisionV;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = divisionV;
        gl_TessLevelOuter[1] = divisionU;
        gl_TessLevelOuter[2] = divisionV;
        gl_TessLevelOuter[3] = divisionU;

        gl_TessLevelInner[0] = divisionU;
        gl_TessLevelInner[1] = divisionV;
    }
}