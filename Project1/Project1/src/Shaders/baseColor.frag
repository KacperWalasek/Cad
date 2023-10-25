#version 410

layout(location = 0) out vec4 FragColor;
uniform vec4 color;

in vec4 gl_FragCoord;

void main()
{
    FragColor = vec4(gl_FragCoord.z,0,0,1);
}