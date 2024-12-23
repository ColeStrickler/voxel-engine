#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in int aBlockType;
//layout (location = 2) in vec2 aTexCoords;
uniform mat4 model;

//out vec3 Normal;
//out vec3 FragPos;
//out vec2 TexCoords;

flat out int BlockType;

/*
    Because normals need to be transformed along with our vertices, we apply that here
    If we do not do this we do not get correct lighting calculations

    LearnOpenGL says this is a costly operation to perform on the shaders and should be done on the CPU
*/
void main()
{

    //if (!( aBlockType >= 0 && aBlockType <= 8))
    //    return;
    int face = (aBlockType >> 16) & 0xffff;
    int type = aBlockType & 0xffff;
    if (type <= 0 || type >= 9)
        return;
    if (face == 0)
        return;
    //if (aBlockType == 1)

    BlockType = aBlockType;

    

    vec3 FragPos = vec3(model * vec4(aPos, 1.0)); // world space * model transform gives us position of fragment
    gl_Position = vec4(FragPos, 1.0);
} 