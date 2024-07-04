#version 330 core
struct Material {
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 color;
};



out vec4 FragColor;

uniform Light light;  
uniform Material material;
uniform vec3 viewPos;


in vec3 Normal;
in vec3 FragPos;
 

void main()
{


    // ambient
    vec3 ambient = material.ambient * light.color;


    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff_coefficent = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (material.diffuse * diff_coefficent) * light.color;


    

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), (material.shininess*128));
    vec3 specular = (material.specular * spec) * light.color;  


    vec3 resColor = (ambient + diffuse + specular);

    FragColor = vec4(resColor, 1.0);
}