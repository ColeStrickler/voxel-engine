#ifndef CHUNK_H
#define CHUNK_H
#include <vector>
#include "glbuffer.h"
#include "glvertexarray.h"
#include "renderobject.h"
#include "world.h"
#include "block.h"
#define MAX_CHUNK_HEIGHT 64
#define MIN_CHUNK_HEIGHT 0
#define CHUNK_WIDTH 16
#define PACK_FACEBLOCK(face, blocktype) ((face << 16) | (blocktype & 0xFFFF))


struct ChunkVertex
{
    float position[3];
    int faceBlockType; // [face|blockType] --> face is upper 16 bits, blockType is lower 16 bits
    int reserved;
    float texCoords[2];
    // We do not need to store texture coords because we will infer them from the block type.
    // Each block gets 4 slots in the texture map, 1 for top, 2 for the sides, 1 for bottom
}__attribute__((packed));



class Chunk
{
public:
    Chunk(int x, int z, ShaderProgram* sp);
    ~Chunk();

    RenderObject* GetRenderObject() { return m_RenderObj; }
private:
    int m_xCoord;
    int m_zCoord;
    void GenerateChunkMesh(ShaderProgram* sp);
    void GenerateChunk();
    void Render();
    void BlockGenVertices(Block& block, float x, float y, float z);
    std::vector<ChunkVertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    Block m_Blocks[16][64][16];
    IndexBuffer* m_IB;
    VertexBuffer* m_VB;
    VertexArray* m_VA;
    RenderObject* m_RenderObj;
};

#endif 