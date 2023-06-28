#version 410

layout(location = 0) out vec4 FragColor;

uniform sampler2D interesectTex;
uniform bool intersect;
uniform bool reverseIntersect;
uniform vec4 color;

in vec2 uv;

void main()
{
    vec4 intersection = texture(interesectTex, uv);
    if(intersect && (intersection.z > 0.5f == reverseIntersect))
        discard;
    FragColor = color;

}