#ifndef CHUNK_H
#define CHUNK_H
#include <vector>
#include <thread>
#include <queue>
#include "gl.h"
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
    Chunk(int x, int z, ShaderProgram*, bool delay);
    ~Chunk();
    glm::vec2 GetPosition();
    RenderObject* GetRenderObject() { return m_RenderObj; }
    void GenerateChunkMesh(ShaderProgram* sp);
private:
    int m_xCoord;
    int m_zCoord;
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


#define CHUNK_DISTANCE static_cast<int>(ViewDistance/CHUNK_WIDTH/4)
#define MAX_CHUNKS ((CHUNK_DISTANCE*4)*(CHUNK_DISTANCE*4)) // safety net
//#define CHUNK_MAP_CENTER (CHUNK_DISTANCE/2)

enum CHUNK_WORKER_CMD
{
    FREE,
    ALLOC,
    UPDATE,
};



struct ChunkWorkItem
{
public:
    ChunkWorkItem(Chunk* nchunk, CHUNK_WORKER_CMD wcmd);
    ChunkWorkItem(int xcoord, int zcoord, CHUNK_WORKER_CMD wcmd);
    int x, z;

    Chunk* chunk;
    CHUNK_WORKER_CMD cmd;
};

#define CHUNK_MANAGER_THREADCOUNT 4
class ChunkManager
{
public:
    ChunkManager();
    ~ChunkManager();

    void PerFrame();

    static void GenChunk(int x, int z);

    void MapMoveForward();
    void MapMoveBackward();
    void MapMoveLeft();
    void MapMoveRight();
    
    std::vector<Chunk*> m_ActiveChunks;
    
    std::vector<std::vector<Chunk*>> m_ChunkMap;
    std::pair<int, int> m_CurrentChunk;
private:
    void AddChunkToRenderer(Chunk* chunk);
    void RemoveChunkFromRenderer(Chunk* chunk);

    static void ChunkWorkerThread();

    static std::condition_variable m_WorkerCV;
    static std::mutex m_WorkerLock;
    static std::mutex m_FinishedItemsLock;
    static std::queue<ChunkWorkItem*> m_WorkItems;
    static std::queue<Chunk*> m_FinishedWork;
    static ShaderProgram* m_ChunkShader;
    static Texture* m_TextureAtlasDiffuse;
    static Texture* m_TextureAtlasSpecular;
    std::thread m_WorkerThreads[CHUNK_MANAGER_THREADCOUNT];
};





#endif 