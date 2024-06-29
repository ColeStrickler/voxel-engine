#version 330 core
out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;


in vec3 Normal;
in vec3 FragPos;
void main()
{
    float specularStrength = 0.5;
    float ambientStrength = 0.1;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff_coefficent = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff_coefficent * lightColor;


    // ambient
    vec3 ambient = ambientStrength * lightColor;


    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float shininess = 256;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;  


    vec3 resColor = (ambient + diffuse + specular) * objectColor;

    FragColor = vec4(resColor, 1.0);
}