#version 410

layout(location = 0) out vec4 FragColor;
uniform vec4 color;
uniform float radius;
uniform bool flatMilling;

in vec4 gl_FragCoord;
in vec2 uv;

void main()
{
    float z = gl_FragCoord.z;
    if (!flatMilling)
    {
        float dist = abs(uv.y - 0.5f);
        float sphereF = radius * ( 1.0f - sqrt(1.0f - dist));
        z = z + sphereF;
    }
    FragColor = vec4(z,0,0,1);
    gl_FragDepth = z;
}