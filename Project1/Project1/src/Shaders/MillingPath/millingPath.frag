#version 410

layout(location = 0) out vec4 FragColor;
uniform vec4 color;
uniform float radius;
uniform bool flatMilling;
uniform float heightWidthRatio;

in vec4 gl_FragCoord;
in vec2 uv;
in vec3 pos;

void main()
{
    float z = pos.z;
    if (!flatMilling)
    {
        float dist = 2.0f*abs(uv.y - 0.5f);
        float sphereF = radius * ( 1.0f - sqrt(1.0f - dist*dist));
        z = z + sphereF;
    }
    FragColor = vec4(z,0,0,1);
    gl_FragDepth = z;
}