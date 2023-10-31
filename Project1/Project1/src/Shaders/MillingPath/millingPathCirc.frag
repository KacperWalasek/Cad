#version 410

layout(location = 0) out vec4 FragColor;
uniform vec4 color;
uniform float radius;
uniform bool flatMilling;

in vec4 gl_FragCoord;
in float dist;
in vec3 pos;

void main()
{
    float z = pos.z;
    if (!flatMilling)
    {
        float sphereF = radius * ( 1.0f - sqrt(1.0f - dist*dist));
        z = z + sphereF;
    }
    FragColor = vec4(z,0,0,1);
    gl_FragDepth = z;
}