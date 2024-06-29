#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;


/*
    Because normals need to be transformed along with our vertices, we apply that here
    If we do not do this we do not get correct lighting calculations

    LearnOpenGL says this is a costly operation to perform on the shaders and should be done on the CPU
*/
vec3 NewNormal()
{
    mat4 modelView = view * model;
    mat3 normalMatrix = transpose(inverse(mat3(modelView)));
    return normalize(normalMatrix * aNormal);
}


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0)); // world space * model transform gives us position of fragment


    Normal = NewNormal();
} 