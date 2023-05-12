#version 410 
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTextureLeft;
uniform sampler2D screenTextureRight;

void main()
{ 
    vec4 c1 = texture(screenTextureLeft, TexCoords);
    vec4 c2 = texture(screenTextureRight, TexCoords);

    FragColor = vec4(c1.x,c2.y,c2.z,1.0f);
}