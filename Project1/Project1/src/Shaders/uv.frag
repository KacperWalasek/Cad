#version 410

layout(location = 0) out vec4 FragColor;

uniform sampler2D interesectTex[10];
uniform bool reverseIntersect[10];
uniform int interesectCount;

uniform vec4 color;

in vec2 uv;

void main()
{
    for(int i = 0; i < interesectCount; i++)
    {
        vec4 intersection = texture(interesectTex[i], uv);
        if(intersection.z > 0.5f == reverseIntersect[i])
            discard;
    }
    FragColor = color;

}