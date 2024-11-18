#ifndef BLOCK_H
#define BLOCK_H

#include "logger.h"
#include <vector>
#include <utility>
#include <assert.h>


#define TEXTURE_ATLAS_WIDTH     1024.0f
#define TEXTURE_ATLAS_HEIGHT    512.0f
#define TEXTURE_WIDTH           16.0f
#define NUMROW ((TEXTURE_ATLAS_HEIGHT/TEXTURE_WIDTH))
#define MAXCOL ((TEXTURE_ATLAS_WIDTH/TEXTURE_WIDTH) - 1)


#define BLOCK_VERTICES_SIZE (sizeof(float)*8*6*4)
#define BLOCK_INDICES_COUNT 6*6

struct BlockVertex {
    float position[3];  // Position in 3D space
    float normal[3];    // Normal vector
    float texCoords[2]; // Texture coordinates
};

/*
    Index into this with block type. Gives the coordinates into the texture atlas (row, column),
    where 1=16px


*/
const std::vector<std::vector<std::pair<uint32_t, uint32_t>>> BlockFaceIndexes = {
    // front,back,left,right,top,bottom
    {},                                         // BLOCKNONE
    {{0,1}, {0,1}, {0,1}, {0,1}, {0,0}, {0,2}}, // DirtSurface
    {{0,2}, {0,2}, {0,2}, {0,2}, {0,2}, {0,2}}, // Dirt
    {{0,3}, {0,3}, {0,3}, {0,3}, {0,3}, {0,3}}, // Stone
    {{0,4}, {0,4}, {0,4}, {0,4}, {0,4}, {0,4}}, // Iron
    {{0,5}, {0,5}, {0,5}, {0,5}, {0,5}, {0,5}}, // Gold
    {{0,6}, {0,6}, {0,6}, {0,6}, {0,6}, {0,6}}, // Coal
    {{0,7}, {0,7}, {0,7}, {0,7}, {0,7}, {0,7}}, // Diamond
    {{0,8}, {0,8}, {0,8}, {0,8}, {0,8}, {0,8}}, // Sand

};










enum class BlockType : short
{
    BLOCKNONE,
    DirtSurface,
    Dirt,
    Stone,
    Iron,
    Gold,
    Coal,
    Diamond,
    Sand,
    OakWood,
    OakLeaf,
    BlueWater,
    BLOCKMAX
};


enum BLOCKFACE 
{
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};


typedef struct BlockUpdate
{
    BlockUpdate();
    BlockUpdate(int x, int y, int z, BlockType block);
    BlockUpdate(const BlockUpdate& other);
    int x;
    int y;
    int z;
    BlockType type;
}BlockUpdate;



class Block
{
public:
    Block();
    ~Block();
    bool isActive() const;
    void setActive(bool bActive);
    void setType(BlockType type);
    BlockType getBlockType() const;
    static std::vector<std::pair<float, float>> GenBlockVertices(BlockType blocktype, BLOCKFACE face);
    static std::vector<std::pair<float,float>> GenTextureCoordBlockFace(std::pair<uint32_t, uint32_t> TextureAtlasIndex);
    uint16_t m_Active_Type; 
private:
    // bool m_bIsActive;
    // BlockType m_Type;

    /*
        We pack both of these values into a single uint16_t so we can save a byte, reducing block size by 50%

        bit 15      => m_bIsActive
        bit 0-14    => m_Type 
    */
    
}__attribute__((packed));

void PrintBlockVertex(BlockVertex& v);


const std::vector<BlockVertex> cubeVertices = {
    // Front Face
    {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // Bottom-left
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // Bottom-right
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // Top-left
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // Top-right

    // Back Face
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // Bottom-left
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // Bottom-right
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // Top-left
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // Top-right

    // Left Face
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom-left
    {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Bottom-right
    {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // Top-left
    {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // Top-right

    // Right Face
    {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Bottom-left
    {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom-right
    {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // Top-left
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // Top-right

    // Top Face
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // Bottom-left
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}, // Bottom-right
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // Top-left
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, // Top-right

    // Bottom Face
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // Bottom-left
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // Bottom-right
    {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // Top-left
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}  // Top-right
};

const std::vector<unsigned int> cubeIndices = {
    // Front Face
    0, 1, 2, 1, 3, 2,

    // Back Face
    4, 5, 6, 5, 7, 6,

    // Left Face
    8, 9, 10, 9, 11, 10,

    // Right Face
    12, 13, 14, 13, 15, 14,

    // Top Face
    16, 17, 18, 17, 19, 18,

    // Bottom Face
    20, 21, 22, 21, 23, 22
};

#endif