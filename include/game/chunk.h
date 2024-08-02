#ifndef CHUNK_H
#define CHUNK_H
#include <vector>
#include <thread>
#include <queue>
#include <unordered_set>
#include "gl.h"
#include "glbuffer.h"
#include "glvertexarray.h"
#include "renderobject.h"
#include "world.h"
#include "block.h"
#define MAX_CHUNK_HEIGHT 96
#define MIN_CHUNK_HEIGHT 0
#define CHUNK_WIDTH 16
#define PACK_FACEBLOCK(face, blocktype) ((face << 16) | (blocktype & 0xFFFF))

extern GLManager gl;

struct ChunkVertex
{
    float position[3];
    int faceBlockType; // [face|blockType] --> face is upper 16 bits, blockType is lower 16 bits
    int reserved;
    float texCoords[2];
    // We do not need to store texture coords because we will infer them from the block type.
    // Each block gets 4 slots in the texture map, 1 for top, 2 for the sides, 1 for bottom
}__attribute__((packed));

std::string pair2String(int x, int y);

class Chunk
{
public:
    Chunk(int x, int z, ShaderProgram* sp);
    Chunk(int x, int z, ShaderProgram*, bool delay);
    ~Chunk();
    glm::vec2 GetPosition();
    std::pair<int, int> GetPositionAsPair();
    std::string GetPositionAsString();
    RenderObject* GetRenderObject() { return m_RenderObj; }
    void GenerateChunkMesh(ShaderProgram* sp);
private:
    int m_xCoord;
    int m_zCoord;
    void GenerateChunk();
    void BlockGenVertices(Block& block, float x, float y, float z);
    std::vector<ChunkVertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    Block m_Blocks[CHUNK_WIDTH][MAX_CHUNK_HEIGHT][CHUNK_WIDTH];
    IndexBuffer* m_IB;
    VertexBuffer* m_VB;
    VertexArray* m_VA;
    RenderObject* m_RenderObj;
};


#define CHUNK_DISTANCE static_cast<int>(ViewDistance/CHUNK_WIDTH/4)
#define DELETE_DISTANCE (CHUNK_DISTANCE*2)
#define MAX_CHUNKS ((CHUNK_DISTANCE*10)*(CHUNK_DISTANCE*10)) // safety net
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

#define CHUNK_MANAGER_THREADCOUNT 8
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
    

    std::pair<int, int> m_CurrentChunk;
private:
    void AddChunkToRenderer(Chunk* chunk);
    void RemoveChunkFromRenderer(Chunk* chunk);

    static void ChunkWorkerThread();
    static std::unordered_set<std::string> m_UsedChunks;
    static std::condition_variable m_WorkerCV;
    static std::mutex m_WorkerLock;
    static std::mutex m_FinishedItemsLock;
    static std::mutex m_ToDeleteLock;
    static std::vector<std::string> m_ToDeleteList;
    static std::queue<ChunkWorkItem*> m_WorkItems;
    static std::queue<Chunk*> m_FinishedWork;
    static ShaderProgram* m_ChunkShader;
    static Texture* m_TextureAtlasDiffuse;
    static Texture* m_TextureAtlasSpecular;
    std::thread m_WorkerThreads[CHUNK_MANAGER_THREADCOUNT];
};





#endif 