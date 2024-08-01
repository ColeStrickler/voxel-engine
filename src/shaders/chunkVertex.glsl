#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in int aFaceBlockType;
layout (location = 2) in int aReserved;
layout (location = 3) in vec2 aTexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

int TEXTURE_MAP_WIDTH = 1024;
int TEXTURE_MAP_HEIGHT = 512;

vec3 GetFaceNormal(int face)
{
    switch (face)
    {
        case 0: return vec3(0.0, 0.0, 1.0); // front
        case 1: return vec3(0.0, 0.0, -1.0); // back
        case 2: return vec3(-1.0, 0.0, 0.0); // left
        case 3: return vec3(1.0, 0.0, 0.0);  // right
        case 4: return vec3(0.0, 1.0, 0.0); // top
        case 5: return vec3(0.0, -1.0, 0.0); // bottom
        default: return vec3(0.0, 0.0, 1.0);
    }
}


/*
    In our setup each block gets 4 16x16 slots,
*/
vec2 GetTextureCoords(int face, int blocktype)
{
    float index;
    switch (face)
    {
        case 0:   index = (1*16)/64; break;// front
        case 1:   index = (1*16)/64; break; // back
        case 2:   index = (1*16)/64; break; // left
        case 3:   index = (1*16)/64; break; // right
        case 4:   index = (0*16)/64; break;// top
        case 5:   index = (2*16)/64; break; // bottom
        default:  index = (0*16)/64; break;    
    }

    int absolute = (blocktype*64);
    float xloc = (absolute % TEXTURE_MAP_WIDTH)/TEXTURE_MAP_WIDTH;
    float yloc = ((absolute/TEXTURE_MAP_WIDTH)*16) / TEXTURE_MAP_WIDTH;




    return vec2(xloc, yloc);

}

/*
    Because normals need to be transformed along with our vertices, we apply that here
    If we do not do this we do not get correct lighting calculations

    LearnOpenGL says this is a costly operation to perform on the shaders and should be done on the CPU
*/

void main()
{
    int face = ((aFaceBlockType >> 16) & 0xFFFF);
    int blocktype = (aFaceBlockType & 0xFFFF);

    FragPos = vec3(model * vec4(aPos, 1.0)); // world space * model transform gives us position of fragment
    gl_Position = projection * view * vec4(FragPos, 1.0);

    Normal = mat3(transpose(inverse(model))) * GetFaceNormal(face);  
    TexCoords = aTexCoords;
} 