#version 410

layout(location = 0) out vec4 FragColor;
uniform vec2 heightRange;
in vec4 pos;

void main()
{
    float z = (pos.z - heightRange.x)/ (heightRange.y - heightRange.x);
    FragColor = vec4(z,z,z,1);
}