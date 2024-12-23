#version 430 core

layout(points) in;                              // Input: points
layout(triangle_strip, max_vertices = 36) out; // Output: triangle strip with up to 36 vertices

flat out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
// Texture Atlas Constants
#define TEXTURE_WIDTH  16.0
#define TEXTURE_ATLAS_WIDTH  1024.0  // Change this to match your atlas size
#define TEXTURE_ATLAS_HEIGHT 512.0  // Change this to match your atlas size
#define NUMROW (TEXTURE_ATLAS_HEIGHT/TEXTURE_WIDTH)  // Number of rows in the texture atlas

flat in int BlockType[];

// Define SSBO layout
layout(std430, binding = 0) buffer data {
    int BlockTextureIndexBuffer[];
};

uniform mat4 view;
uniform mat4 projection;

#define FACE_FRONT 0
#define FACE_BACK 1
#define FACE_LEFT 2
#define FACE_RIGHT 3
#define FACE_TOP 4
#define FACE_BOTTOM 5

int CORNER_BL =0;
int CORNER_BR =1;
int CORNER_TL =2;
int CORNER_TR =3;


int PerBlockSize = 12;

// Define the offsets for a unit cube
const vec3 offsets[8] = vec3[](
    vec3(-0.5, -0.5, -0.5), // 0: bottom-left-front
    vec3( 0.5, -0.5, -0.5), // 1: bottom-right-front
    vec3( 0.5,  0.5, -0.5), // 2: top-right-front
    vec3(-0.5,  0.5, -0.5), // 3: top-left-front
    vec3(-0.5, -0.5,  0.5), // 4: bottom-left-back
    vec3( 0.5, -0.5,  0.5), // 5: bottom-right-back
    vec3( 0.5,  0.5,  0.5), // 6: top-right-back
    vec3(-0.5,  0.5,  0.5)  // 7: top-left-back
);

// Indices for cube faces (two triangles per face)
const int indices[36] = int[](
    0, 1, 2, 2, 3, 0, // Front face
    1, 5, 6, 6, 2, 1, // Right face
    5, 4, 7, 7, 6, 5, // Back face
    4, 0, 3, 3, 7, 4, // Left face
    3, 2, 6, 6, 7, 3, // Top face
    4, 5, 1, 1, 0, 4  // Bottom face
);

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


vec4 GetFaceTextureCoords(int trow, int tcol)
{
    float row = (NUMROW - (trow));
    float col = (tcol);

    float xmin =   (col*TEXTURE_WIDTH)/TEXTURE_ATLAS_WIDTH;
    float ymin =   ((row-1.0f)*TEXTURE_WIDTH)/TEXTURE_ATLAS_HEIGHT;
    float xmax =   ((col+1.0f)*TEXTURE_WIDTH)/TEXTURE_ATLAS_WIDTH;
    float ymax =   ((row)*TEXTURE_WIDTH)/TEXTURE_ATLAS_HEIGHT;

    return vec4(xmin, ymin, xmax, ymax);
}





vec2 GetFaceTextureCoords(int corner, vec4 coords)
{
    //GetFaceTextureCoords() = vec4(xmin, ymin, xmax, ymax)
    switch(corner)
    {
        case 0: return vec2(coords[0], coords[1]); // bottom left
        case 1: return vec2(coords[2], coords[1]); // bottom right
        case 2: return vec2(coords[0], coords[3]); // top left
        case 3: return vec2(coords[2], coords[3]); // top right
        default:
            return vec2(0.0, 0.0);
    }
}

void GenVertex(vec4 FaceTexCoords, vec4 point, int index, int face, int corner)
{
    Normal = GetFaceNormal(face);
    vec3 pos = vec3(point) + offsets[indices[index]];
    FragPos = pos;
    gl_Position = projection*view*vec4(pos,1.0);
    TexCoords = GetFaceTextureCoords(corner, FaceTexCoords);
    EmitVertex();
}





void main()
{
    
    // We are only passing in a single point and creating a vertex from there
    vec4 point = gl_in[0].gl_Position;
    
    int fb = BlockType[0];
    
    int face = ((fb >> 16) & 0xFFFF);
    int blockType = (fb & 0xFFFF);
    if (face == 0 || blockType == 0)
        return;
    // blockType-1 because there is no BLOCKNONE after we flatten and send to GPU
    int startBlockTexIndex = (blockType-1)*PerBlockSize;


    
    




    /* 
        Front Face
    */
    bool check = (1 & (face >> FACE_FRONT)) != 0;
    if (check)
    {
        int frontFaceTextureRow = BlockTextureIndexBuffer[startBlockTexIndex + 0];
        int frontFaceTextureCol = BlockTextureIndexBuffer[startBlockTexIndex + 1];
        vec4 frontFaceTexCoords = GetFaceTextureCoords(frontFaceTextureRow, frontFaceTextureCol);

        GenVertex(frontFaceTexCoords, point, 0, FACE_FRONT, CORNER_BL);
        GenVertex(frontFaceTexCoords, point, 1, FACE_FRONT, CORNER_BR);
        GenVertex(frontFaceTexCoords, point, 2, FACE_FRONT, CORNER_TR);
        EndPrimitive();
        GenVertex(frontFaceTexCoords, point, 3, FACE_FRONT, CORNER_TR);
        GenVertex(frontFaceTexCoords, point, 4, FACE_FRONT, CORNER_TL);
        GenVertex(frontFaceTexCoords, point, 5, FACE_FRONT, CORNER_BL);
        EndPrimitive();
    }
    
    
    /*
        Back Face
    */
    if ((1 & (face >> FACE_BACK)) != 0)
    {
        int backFaceTextureRow = BlockTextureIndexBuffer[startBlockTexIndex + 2];
        int backFaceTextureCol = BlockTextureIndexBuffer[startBlockTexIndex + 3];
        
        vec4 backFaceTexCoords = GetFaceTextureCoords(backFaceTextureRow, backFaceTextureCol);
        GenVertex(backFaceTexCoords, point, 6, FACE_BACK, CORNER_BL);
        GenVertex(backFaceTexCoords, point, 7, FACE_BACK, CORNER_BR);
        GenVertex(backFaceTexCoords, point, 8, FACE_BACK, CORNER_TR);
        EndPrimitive();
        GenVertex(backFaceTexCoords, point, 9, FACE_BACK, CORNER_TR);
        GenVertex(backFaceTexCoords, point, 10, FACE_BACK, CORNER_TL);
        GenVertex(backFaceTexCoords, point, 11, FACE_BACK, CORNER_BL);
        EndPrimitive();
    }

    /*
        Left Face
    */
    if ((1 & (face >> FACE_LEFT)) != 0)
    {
        int leftFaceTextureRow = BlockTextureIndexBuffer[startBlockTexIndex + 4];
        int leftFaceTextureCol = BlockTextureIndexBuffer[startBlockTexIndex + 5];
        vec4 leftFaceTexCoords = GetFaceTextureCoords(leftFaceTextureRow, leftFaceTextureCol);
        GenVertex(leftFaceTexCoords, point, 12,  FACE_LEFT, CORNER_BL);
        GenVertex(leftFaceTexCoords, point, 13,  FACE_LEFT, CORNER_BR);
        GenVertex(leftFaceTexCoords, point, 14,  FACE_LEFT, CORNER_TR);
        EndPrimitive();
        GenVertex(leftFaceTexCoords, point, 15,  FACE_LEFT, CORNER_TR);
        GenVertex(leftFaceTexCoords, point, 16, FACE_LEFT, CORNER_TL);
        GenVertex(leftFaceTexCoords, point, 17, FACE_LEFT, CORNER_BL);
        EndPrimitive();
    }
    

    /*
        Right Face
    */
    if ((1 & (face >> FACE_RIGHT)) != 0)
    {
        int rightFaceTextureRow = BlockTextureIndexBuffer[startBlockTexIndex + 6];
        int rightFaceTextureCol = BlockTextureIndexBuffer[startBlockTexIndex + 7];
        vec4 rightFaceTexCoords = GetFaceTextureCoords(rightFaceTextureRow, rightFaceTextureCol);
        GenVertex(rightFaceTexCoords, point, 18,  FACE_RIGHT, CORNER_BL);
        GenVertex(rightFaceTexCoords, point, 19,  FACE_RIGHT, CORNER_BR);
        GenVertex(rightFaceTexCoords, point, 20,  FACE_RIGHT, CORNER_TR);
        EndPrimitive();
        GenVertex(rightFaceTexCoords, point, 21,  FACE_RIGHT, CORNER_TR);
        GenVertex(rightFaceTexCoords, point, 22,  FACE_RIGHT, CORNER_TL);
        GenVertex(rightFaceTexCoords, point, 23,  FACE_RIGHT, CORNER_BL);
        EndPrimitive();
    }
    


    /*
        Top Face
    */
    if ((1 & (face >> FACE_TOP)) != 0)
    {
        int  topFaceTextureRow = BlockTextureIndexBuffer[startBlockTexIndex + 8];
        int  topFaceTextureCol = BlockTextureIndexBuffer[startBlockTexIndex + 9];
        vec4 topFaceTexCoords = GetFaceTextureCoords(topFaceTextureRow, topFaceTextureCol);
        GenVertex(topFaceTexCoords, point, 24,  FACE_TOP, CORNER_BL);
        GenVertex(topFaceTexCoords, point, 25,  FACE_TOP, CORNER_BR);
        GenVertex(topFaceTexCoords, point, 26,  FACE_TOP, CORNER_TR);
        EndPrimitive();
        GenVertex(topFaceTexCoords, point, 27,  FACE_TOP, CORNER_TR);
        GenVertex(topFaceTexCoords, point, 28,  FACE_TOP, CORNER_TL);
        GenVertex(topFaceTexCoords, point, 29,  FACE_TOP, CORNER_BL);
        EndPrimitive();
    }
    


    /*
        Bottom Face
    */
    if ((1 & (face >> FACE_BOTTOM)) != 0)
    {
        int  bottomFaceTextureRow = BlockTextureIndexBuffer[startBlockTexIndex + 10];
        int  bottomFaceTextureCol = BlockTextureIndexBuffer[startBlockTexIndex + 11];
        vec4 bottomFaceTexCoords = GetFaceTextureCoords(bottomFaceTextureRow, bottomFaceTextureCol);
        GenVertex(bottomFaceTexCoords, point, 30,  FACE_BOTTOM, CORNER_BL);
        GenVertex(bottomFaceTexCoords, point, 31,  FACE_BOTTOM, CORNER_BR);
        GenVertex(bottomFaceTexCoords, point, 32,  FACE_BOTTOM, CORNER_TR);
        EndPrimitive();
        GenVertex(bottomFaceTexCoords, point, 33,  FACE_BOTTOM, CORNER_TR);
        GenVertex(bottomFaceTexCoords, point, 34,  FACE_BOTTOM, CORNER_TL);
        GenVertex(bottomFaceTexCoords, point, 35,  FACE_BOTTOM, CORNER_BL);
        EndPrimitive();
    }
    
}