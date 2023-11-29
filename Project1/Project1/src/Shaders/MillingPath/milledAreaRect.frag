#version 410

layout(location = 0) out vec4 FragColor;
uniform vec4 color;
uniform float radius;
uniform bool flatMilling;
uniform sampler2D tex;
uniform float treshold;
uniform int stepIndex;
uniform int stepCount;

in vec4 gl_FragCoord;
in vec2 uv;
in vec3 pos;

void main()
{
    float currentZ = texture(tex,(pos.xy+1.0f)/2.0f).x;
    float z = pos.z;
    if (!flatMilling)
    {
        float dist = 2.0f*abs(uv.y - 0.5f);
        float sphereF = radius * ( 1.0f - sqrt(1.0f - dist*dist));
        z = z + sphereF;
    }
    if (z > currentZ)
        discard;
    if (abs(currentZ - z) < 0.001f)
        discard;

    float ind = stepIndex/float(stepCount);
    FragColor = vec4(ind,ind,ind,1.0f);
}