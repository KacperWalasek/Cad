#version 410

layout(location = 0) out vec4 FragColor;
uniform vec4 color;
uniform vec4 lightColor;

in vec3 lightVec;
in vec3 viewVec;
in vec4 worldPos;
 
void main()
{
    float dxdu = dFdx(worldPos.x);
    float dxdv = dFdy(worldPos.x);

    float dydu = dFdx(worldPos.y);
    float dydv = dFdy(worldPos.y);

    float dzdu = dFdx(worldPos.z);
    float dzdv = dFdy(worldPos.z);

    vec3 normal = normalize(cross(vec3(dxdu,dydu,dzdu), vec3(dxdv,dydv,dzdv)));
  
    // ambient
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * color.xyz;    
     
     // diffuse 
    float diff = max(dot(normal, lightVec), 0.0);
    vec3 diffuse = 0.7 * lightColor.xyz * color.xyz * diff;
    
    // specular
	vec3 halfVec = normalize(viewVec + lightVec);
    vec3 specular = lightColor.xyz * 0.5f * pow(max(0,dot(normal,halfVec)),4);
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 