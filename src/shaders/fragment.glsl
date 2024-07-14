#version 330 core
struct Material {
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 


struct TextureObject
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};




out vec4 FragColor;
uniform  DirLight dirLight;
#define MAX_POINT_LIGHTS 100
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int UsedPointLights;
uniform Material material;
uniform TextureObject textureObject;
uniform vec3 viewPos;
uniform int ObjectType;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;



vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), textureObject.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(textureObject.diffuseMap, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(textureObject.diffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(textureObject.specularMap, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), textureObject.shininess);

    /*
        CURRENTLY HAVE NO ATTENUATION ON DIRECTIONAL LIGHT SOURCES
    */


    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(textureObject.diffuseMap, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(textureObject.diffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(textureObject.specularMap, TexCoords));
    return (ambient + diffuse + specular);
} 


vec3 getTextureColor()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < UsedPointLights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir); 
    
    return result;
}

void main()
{
    switch(ObjectType)
    {
        case 0: FragColor = vec4(vec3(1.0, 1.0, 1.0), 1.0); break; // not implemented
        case 1: FragColor = vec4(vec3(1.0, 1.0, 1.0), 1.0); break;
        case 2: FragColor = vec4(getTextureColor(), 1.0); break;
        case 3: FragColor = vec4(vec3(1.0, 1.0, 1.0), 1.0); break;
        case 5: FragColor = vec4(vec3(texture(textureObject.diffuseMap, TexCoords)), 1.0); break;
        default: FragColor = vec4(1.0, 1.0, 1.0, 1.0); break;
    }
}