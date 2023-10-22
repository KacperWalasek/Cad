#version 410

layout(location = 0) out vec4 FragColor;

uniform sampler2D tex;
uniform vec4 color;

in vec2 TexCoords;
in vec2 uv;

void main()
{
    vec4 intersection = texture(tex, uv);
    FragColor = normalize(intersection);
}