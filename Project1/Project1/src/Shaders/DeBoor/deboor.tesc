#version 410 core

layout (vertices=2) out;
patch out vec4 bp0;
patch out vec4 bp1;
patch out vec4 bp2;
patch out vec4 bp3;

uniform mat4 transform;
uniform float t0;
uniform float t1;

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
        bp0 = gl_in[0].gl_Position;
        bp1 = gl_in[1].gl_Position;
        bp2 = gl_in[2].gl_Position;
        bp3 = gl_in[3].gl_Position;

        float d = 0;
        for(int i = 0; i < 3; i++)
        {
            vec4 p0 = transform*gl_in[i].gl_Position;
            p0 /= p0.w;
            vec4 p1 = transform*gl_in[i+1].gl_Position;
            p1 /= p1.w;
            d += sqrt(pow(p1.x-p0.x,2) + pow(p1.y-p0.y,2) + pow(p1.z-p0.z,2));
        }   
        float division = clamp(ceil(d*5), 4,200);
        gl_TessLevelOuter[0] = 1;
        gl_TessLevelOuter[1] = division;

    }
}