#ifndef CHUNK_H
#define CHUNK_H
#include <vector>
#include <thread>
#include <queue>
#include <unordered_set>
#include <map>
#include "gl.h"
#include "glbuffer.h"
#include "glvertexarray.h"
#include "renderobject.h"
#include "world.h"
#include "block.h"
#include "biome.h"
#include "structures.h"
#include "gpu_allocator.h"
#include "FastNoiseLite.h"
#define MAX_CHUNK_HEIGHT 96
#define DEFAULT_CHUNK_GROUND 64
#define MIN_CHUNK_HEIGHT 0
#define CHUNK_WIDTH 16
#define PACK_FACEBLOCK(face, blocktype) (((face & 0xFF) << 24) | ((unsigned short)blocktype & 0xFFFF))
#define DEFAULT_NOISE_SEED 1337
#define CAN_ALLOC_CHUNK (ChunkManager::m_ActiveChunks.size() < MAX_CHUNKS)
#define IS_IN_CHUNK(x, y, z) ((x >= 0 && x < CHUNK_WIDTH) && (z >= 0 && z < CHUNK_WIDTH) && (y >= 0 && y < MAX_CHUNK_HEIGHT))


// Stand-alone methods
std::string pair2String(int x, int y);



/*
    ORE GENERATION RATE
*/
static const std::vector<std::vector<int>> OreDirections =  {
        {0, 0, 0},
        {1, 0, 0}, {0, 1, 0}, {0, 0, 1},
        {-1, 0, 0}, {0, -1, 0}, {0, 0, -1},
        {1, 1, 0}, {1, 0, 1}, {0, 1, 1},
        {-1, -1, 0}, {-1, 0, -1}, {0, -1, -1},
        {1, -1, 0}, {1, 0, -1}, {0, 1, -1},
        {-1, 1, 0}, {-1, 0, 1}, {0, -1, 1},
        {1, 1, 1}, {1, 1, -1}, {1, -1, 1},
        {1, -1, -1}, {-1, 1, 1}, {-1, 1, -1},
        {-1, -1, 1}, {-1, -1, -1}
};
#define ORE_PASS_THRESHOLD 0.98f
#define ORE_IRON_MAX_VEIN 9
#define ORE_IRON_MIN_DEPTH 48
#define ORE_IRON_GEN_RATE (0.92f)
#define ORE_CAN_GEN_IRON(noise, y) ((noise > ORE_IRON_GEN_RATE) && (y < ORE_IRON_MIN_DEPTH))


#define ORE_GOLD_MAX_VEIN 6
#define ORE_GOLD_MIN_DEPTH 32
#define ORE_GOLD_GEN_RATE (ORE_IRON_GEN_RATE-.02f)
#define ORE_CAN_GEN_GOLD(noise, y) ((noise > ORE_GOLD_GEN_RATE) && (y < ORE_GOLD_MIN_DEPTH))


#define ORE_COAL_MAX_VEIN 30
#define ORE_COAL_MIN_DEPTH 48
#define ORE_COAL_GEN_RATE (ORE_GOLD_GEN_RATE - 0.12f)
#define ORE_CAN_GEN_COAL(noise, y) ((noise > ORE_COAL_GEN_RATE) && (y < ORE_COAL_MIN_DEPTH))

#define ORE_DIAMOND_MAX_VEIN 12
#define ORE_DIAMOND_MIN_DEPTH 16
#define ORE_DIAMOND_GEN_RATE (ORE_COAL_GEN_RATE - 0.001f)
#define ORE_CAN_GEN_DIAMOND(noise, y) ((noise > ORE_DIAMOND_GEN_RATE) && (y < ORE_DIAMOND_MIN_DEPTH))

#define ORE_STONE_MAX_VEIN 25
#define ORE_STONE_MIN_DEPTH 56
#define ORE_STONE_GEN_RATE (ORE_DIAMOND_GEN_RATE - 0.005f)
#define ORE_CAN_GEN_STONE(noise, y) ((noise > ORE_STONE_GEN_RATE) && (y < ORE_STONE_MIN_DEPTH))




struct ChunkVertex
{
    float position[3];
    int face; // [face|blockType] --> face is upper 16 bits, blockType is lower 16 bits
    //int reserved;
    float texCoords[2];
    // We do not need to store texture coords because we will infer them from the block type.
    // Each block gets 4 slots in the texture map, 1 for top, 2 for the sides, 1 for bottom
}__attribute__((packed));


std::vector<ChunkVertex> GetFrontFace(float x, float y, float z, BlockType type);
std::vector<ChunkVertex> GetBackFace(float x, float y, float z, BlockType type);
std::vector<ChunkVertex> GetLeftFace(float x, float y, float z, BlockType type);
std::vector<ChunkVertex> GetRightFace(float x, float y, float z, BlockType type);
std::vector<ChunkVertex> GetTopFace(float x, float y, float z, BlockType type);
std::vector<ChunkVertex> GetBottomFace(float x, float y, float z, BlockType type);
std::vector<ChunkVertex> GetFace(BLOCKFACE face, BlockType type, float x, float y, float z);
std::vector<unsigned int> GetFaceInidices(BLOCKFACE face);

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
    bool isActive(int x, int y, int z);
    bool m_bHasDiamond;


    static BIOMETYPE BiomeSelect(float biomeNoise);
private:
    int m_xCoord;
    int m_zCoord;
    
    void GenerateChunk();
    void BlockGenVertices(BlockType type, float x, float y, float z, BLOCKFACE face);
    static void OrePopulatePass(std::vector<int> coordStart, Chunk* chunk);
    static void OrePassFill(std::vector<int> coordStart, BlockType ore, Chunk* chunk);
    static void AddStructure(Chunk *chunk, int x, int y, int z, STRUCTURETYPE structure, std::vector<Block*>& actives, std::vector<std::vector<int>>& activeCoords);
    
    static int OreGetVeinSize(BlockType ore);
    std::vector<ChunkVertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    Block m_Blocks[CHUNK_WIDTH][MAX_CHUNK_HEIGHT][CHUNK_WIDTH];
    IndexBuffer* m_IB;
    VertexBuffer* m_VB;
    VertexArray* m_VA;
    RenderObject* m_RenderObj;
    static BlockType GetBlockType(int x, int y, int z, int surface, BIOMETYPE biome);
};




#define CHUNK_DISTANCE_ (ViewDistance/CHUNK_WIDTH/2)
#define DELETE_DISTANCE_ (CHUNK_DISTANCE*1.5)
#define MEMORY_LIMIT_ (1024UL*1024UL*1024UL*4UL)
#define VERTICES_PER_CUBE (24)
#define MAX_CUBES_PER_CHUNK ((CHUNK_WIDTH*MAX_CHUNK_HEIGHT*4)+(CHUNK_WIDTH*CHUNK_WIDTH*2))
#define MAX_VERTICES_PER_CHUNK (VERTICES_PER_CUBE*MAX_CUBES_PER_CHUNK)
#define CHUNK_VERTICES_SIZE (sizeof(ChunkVertex)*MAX_VERTICES_PER_CHUNK)

#define INDICES_PER_CUBE (36)
#define CHUNK_INDICES_SIZE (INDICES_PER_CUBE*MAX_CUBES_PER_CHUNK*sizeof(unsigned int))
#define CHUNK_SIZE_OTHER (sizeof(IndexBuffer)+sizeof(VertexBuffer)+sizeof(VertexArray)+sizeof(RenderObject))
#define CHUNK_TWEAK_PARAMETER 6
#define MAX_CHUNKS_ (MEMORY_LIMIT_/ (CHUNK_TWEAK_PARAMETER*(sizeof(Chunk) + CHUNK_VERTICES_SIZE + CHUNK_INDICES_SIZE + CHUNK_SIZE_OTHER))) // safety net
#define MAX_WORK_ITEMS 700
#define ACTIVE_WORK_ITEMS (m_WorkItems.size())
#define CHUNK_WORKER_QUEUE_FULL (ACTIVE_WORK_ITEMS >= MAX_WORK_ITEMS) // this will allow us to catch up with deletions

enum CHUNK_WORKER_CMD
{
    FREE,
    ALLOC,
    UPDATE,
    STRUCTURE_ADD,
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

#define CHUNK_MANAGER_THREADCOUNT 16
class ChunkManager
{
public:
    ChunkManager();
    ~ChunkManager();

    void PerFrame();

    static void GenChunk(int x, int z);
    static void CleanFarChunks();
    static void CleanFarChunks(float div);
    void MapMove();
    

    static RenderObject* m_RenderObj;
    static VertexArray* m_VA;
    static GPUAllocator* m_GPUMemoryManager;
    static std::vector<Chunk*> m_ActiveChunks;
    static std::pair<int, int> m_CurrentChunk;
    static FastNoiseLite m_ChunkHeightNoise;
    static FastNoiseLite m_BiomeNoise;
    static FastNoiseLite m_StructureNoise;
    void AddChunkToRenderer(Chunk* chunk);
    void RemoveChunkFromRenderer(Chunk* chunk);
    static void ChunkWorkerThread();
    
    static std::unordered_map<std::string, Chunk*> m_UsedChunks;
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
private:
    static glm::vec2 CurrentChunkToVec2();
    static float DistanceFromCurrentChunk(Chunk* chunk);
    static float DistanceFromCurrentChunk(int x, int z);
};





#endif 