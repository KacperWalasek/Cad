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
in vec2 textureUV;

void main()
{
    float currentZ = texture(tex,(textureUV+1.0f)/2.0f).x;
    float z = gl_FragCoord.z;
    if (!flatMilling)
    {
        float dist = 2.0f*abs(uv.y - 0.5f);
        float sphereF = radius * ( 1.0f - sqrt(1.0f - dist*dist));
        z = z + sphereF;
    }
    if (z > currentZ)
        discard;
    if (currentZ - z < treshold)
        discard;
    FragColor = vec4(stepIndex/float(stepCount),0.0f,0.0f,1.0f);
}