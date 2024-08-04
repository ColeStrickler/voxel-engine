#include "chunk.h"



extern GLManager gl;
extern Logger logger;
extern Renderer renderer;

int MAX_CHUNKS = MAX_CHUNKS_;
float CHUNK_DISTANCE = CHUNK_DISTANCE_;
float DELETE_DISTANCE = DELETE_DISTANCE_;

std::condition_variable ChunkManager::m_WorkerCV;
std::mutex ChunkManager::m_WorkerLock;
std::mutex ChunkManager::m_FinishedItemsLock;
std::queue<ChunkWorkItem*> ChunkManager::m_WorkItems;
std::mutex ChunkManager::m_ToDeleteLock;
std::vector<std::string> ChunkManager::m_ToDeleteList;
std::queue<Chunk*> ChunkManager::m_FinishedWork;
std::unordered_set<std::string> ChunkManager::m_UsedChunks;
ShaderProgram* ChunkManager::m_ChunkShader;
Texture* ChunkManager::m_TextureAtlasDiffuse;
Texture* ChunkManager::m_TextureAtlasSpecular;
FastNoiseLite ChunkManager::m_ChunkHeightNoise;
std::vector<Chunk*> ChunkManager::m_ActiveChunks;
std::pair<int, int> ChunkManager::m_CurrentChunk;
int init_count = 0;
int delete_count = 0;


#define DEFAULT_CHUNK_GROUND 64
#define HEIGHT_SCALE 96.0f // multiplier to adjust terrain
#define SCALE 0.1f

Chunk::Chunk(int x, int z, ShaderProgram* sp) : m_xCoord(x), m_zCoord(z)
{
    
    GenerateChunk();
    GenerateChunkMesh(sp);
}

Chunk::Chunk(int x, int z, ShaderProgram* sp, bool delay): m_xCoord(x), m_zCoord(z)
{
    GenerateChunk();

    if (!delay)
        GenerateChunkMesh(sp);
}

Chunk::~Chunk()
{
   //delete m_RenderObj; // renderObj deletes VertexArray deletes (indexBuffer, vertexBuffer)
}

glm::vec2 Chunk::GetPosition()
{
    return glm::vec2(static_cast<float>(m_xCoord), static_cast<float>(m_zCoord));
}

std::pair<int, int> Chunk::GetPositionAsPair()
{
    return std::pair<int, int>(m_xCoord, m_zCoord);
}

std::string Chunk::GetPositionAsString()
{
    return pair2String(m_xCoord, m_zCoord);
}

void Chunk::GenerateChunkMesh(ShaderProgram* sp)
{

    BufferLayout* vertex_layout = new BufferLayout({new BufferElement("COORDS", ShaderDataType::Float3, false),
          new BufferElement("faceBlockType", ShaderDataType::Int, false), \
           new BufferElement("texCoord", ShaderDataType::Float2, false)});
    m_IB = new IndexBuffer(m_Indices.data(), m_Indices.size());
    m_VB = new VertexBuffer((float*)m_Vertices.data(), m_Vertices.size()*sizeof(ChunkVertex));
    
    m_VB->SetLayout(vertex_layout);
    m_VA = new VertexArray();
    m_VA->AddVertexBuffer(m_VB);
    m_VA->AddIndexBuffer(m_IB);
    m_RenderObj = new RenderObject(m_VA, m_VB, sp, m_IB, OBJECTYPE::ChunkMesh);
    m_RenderObj->Translate(glm::vec3(static_cast<float>(m_xCoord*CHUNK_WIDTH), 0.0f, static_cast<float>(m_zCoord*CHUNK_WIDTH)));
}

void Chunk::BlockGenVertices(Block &block, float x, float y, float z)
{
   // printf("here %d, %d, %d\n", x, y, z);
    auto type = block.getBlockType();
    std::vector<unsigned int> cubeIndices = {
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
    for (int i = 0; i < cubeIndices.size(); i++)
        cubeIndices[i] += m_Vertices.size();
    m_Indices.insert(m_Indices.end(), cubeIndices.begin(), cubeIndices.end());

    
    auto tex_front = Block::GenBlockVertices(block.getBlockType(), BLOCKFACE::FRONT);
    auto tex_back = Block::GenBlockVertices(block.getBlockType(), BLOCKFACE::BACK);
    auto tex_left = Block::GenBlockVertices(block.getBlockType(), BLOCKFACE::LEFT);
    auto tex_right = Block::GenBlockVertices(block.getBlockType(), BLOCKFACE::RIGHT);
    auto tex_top = Block::GenBlockVertices(block.getBlockType(), BLOCKFACE::TOP);
    auto tex_bottom = Block::GenBlockVertices(block.getBlockType(), BLOCKFACE::BOTTOM);

    std::vector<ChunkVertex> vertvec = {
        // Front Face      
        ChunkVertex {{x+-0.5f, y+-0.5f,  z+0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), {tex_front[0].first, tex_front[0].second}}, // Bottom-left
        ChunkVertex {{x+ 0.5f, y+-0.5f,  z+0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), {tex_front[1].first, tex_front[1].second}}, // Bottom-right
        ChunkVertex {{x+-0.5f, y+ 0.5f,  z+0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), {tex_front[2].first, tex_front[2].second}}, // Top-left
        ChunkVertex {{x+ 0.5f, y+ 0.5f,  z+0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), {tex_front[3].first, tex_front[3].second}}, // Top-right
        // Back Face
        ChunkVertex {{x+-0.5f, y+-0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), {tex_back[0].first, tex_back[0].second}}, // Bottom-left
        ChunkVertex {{x+ 0.5f, y+-0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), {tex_back[1].first, tex_back[1].second}}, // Bottom-right
        ChunkVertex {{x+-0.5f, y+ 0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), {tex_back[2].first, tex_back[2].second}}, // Top-left
        ChunkVertex {{x+ 0.5f, y+ 0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), {tex_back[3].first, tex_back[3].second}}, // Top-right
        // Left Face
        ChunkVertex{{x+-0.5f, y+-0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), {tex_left[0].first, tex_left[0].second}}, // Bottom-left
        ChunkVertex{{x+-0.5f, y+-0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), {tex_left[1].first, tex_left[1].second}}, // Bottom-right
        ChunkVertex{{x+-0.5f, y+ 0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), {tex_left[2].first, tex_left[2].second}}, // Top-left
        ChunkVertex{{x+-0.5f, y+ 0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), {tex_left[3].first, tex_left[3].second}}, // Top-right
        // Right Face
        ChunkVertex{{x+0.5f, y+-0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), {tex_right[0].first, tex_right[0].second}}, // Bottom-left
        ChunkVertex{{x+0.5f, y+-0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), {tex_right[1].first, tex_right[1].second}}, // Bottom-right
        ChunkVertex{{x+0.5f, y+ 0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), {tex_right[2].first, tex_right[2].second}}, // Top-left
        ChunkVertex{{x+0.5f, y+ 0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), {tex_right[3].first, tex_right[3].second}}, // Top-right
        // Top Face
        ChunkVertex {{x+-0.5f,  y+0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), {tex_top[0].first, tex_top[0].second}}, // Bottom-left
        ChunkVertex {{x+ 0.5f,  y+0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), {tex_top[1].first, tex_top[1].second}}, // Bottom-right
        ChunkVertex {{x+-0.5f,  y+0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), {tex_top[2].first, tex_top[2].second}}, // Top-left
        ChunkVertex {{x+ 0.5f,  y+0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), {tex_top[3].first, tex_top[3].second}}, // Top-right
        // Bottom Face
        ChunkVertex{{x+-0.5f, y+-0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), {tex_bottom[0].first, tex_bottom[0].second}}, // Bottom-left
        ChunkVertex{{x+ 0.5f, y+-0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), {tex_bottom[1].first, tex_bottom[1].second}}, // Bottom-right
        ChunkVertex{{x+-0.5f, y+-0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), {tex_bottom[2].first, tex_bottom[2].second}}, // Top-left
        ChunkVertex{{x+ 0.5f, y+-0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), {tex_bottom[3].first, tex_bottom[3].second}}, // Top-right
    };
    m_Vertices.insert(m_Vertices.end(), vertvec.begin(), vertvec.end());
}

BlockType Chunk::GetBlockType(int x, int y, int z, int surface, BIOMETYPE biome)
{
    switch (biome)
    {
        case BIOMETYPE::HILLS:
            return BIOME::Hills_GetBlockType(y, surface);
        case BIOMETYPE::PLAINS:
        {
            return BlockType::Dirt;
        }
        default:
        {
            logger.Log(LOGTYPE::ERROR, "Chunk::GetBlockType() --> Got invalid BIOME.");
            return BlockType::Dirt;
        }
    }
}

void Chunk::GenerateChunk()
{
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
            BIOMETYPE biome = BIOMETYPE::HILLS;
            float noise = ChunkManager::m_ChunkHeightNoise.GetNoise(static_cast<float>(x+m_xCoord*CHUNK_WIDTH), static_cast<float>(z+m_zCoord*CHUNK_WIDTH));
            int surface = static_cast<int>(DEFAULT_CHUNK_GROUND+(noise*BIOME::GetSurfaceVariation(biome)));
            for (int y = 0; y < surface; y++)
            {
                auto& block = m_Blocks[x][y][z];
                block.setType(GetBlockType(x, y, z, surface-1, biome));
                if (x == 0 || z == 0 || x == 15 || z == 15 || y==0 || y == surface-1)
                    block.setActive(true);
                if (block.isActive())
                    BlockGenVertices(block, x, y, z);
            }
        }
    }
   
}

ChunkManager::ChunkManager()
{

    m_ChunkHeightNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    for (int i = 0; i < CHUNK_MANAGER_THREADCOUNT; i++)
    {
        m_WorkerThreads[i] = std::thread(ChunkWorkerThread);
    }
    


    Shader chunk_vertex_shader(util::getcwd() + "/src/shaders/chunkVertex.glsl", GL_VERTEX_SHADER);
    if (chunk_vertex_shader.CheckError() != ShaderError::NO_ERROR_OK)
        logger.Log(LOGTYPE::ERROR, chunk_vertex_shader.FetchLog());
    
    Shader fragment_shader(util::getcwd() + "/src/shaders/fragment.glsl", GL_FRAGMENT_SHADER);
    if (fragment_shader.CheckError() != ShaderError::NO_ERROR_OK)
        logger.Log(LOGTYPE::ERROR, fragment_shader.FetchLog());

    
    m_ChunkShader = new ShaderProgram();
    m_ChunkShader->AddShader(&chunk_vertex_shader);
    m_ChunkShader->AddShader(&fragment_shader);
    if (!m_ChunkShader->Compile())
    {
        logger.Log(LOGTYPE::ERROR, "ChunkManager::ChunkManager() --> Unable to compile Chunk Shader.");
        logger.Log(LOGTYPE::ERROR, "Error #" + std::to_string(m_ChunkShader->CheckError()));
        while(1);
    }
    m_TextureAtlasSpecular = new Texture("/home/cole/Documents/voxel-engine/src/textures/texture_atlas.png", "spec");
    m_TextureAtlasDiffuse = new Texture("/home/cole/Documents/voxel-engine/src/textures/texture_atlas.png", "diff");


    // can see CHUNK_DISTANCE each way
    //auto chunk = new Chunk(0, 0, m_ChunkShader); 
    //AddChunkToRenderer(chunk);
    //m_ActiveChunks.push_back(chunk);

    for (int x = -CHUNK_DISTANCE; x < CHUNK_DISTANCE; x++)
    {
        for (int z = -CHUNK_DISTANCE; z < CHUNK_DISTANCE; z++)
        {
            ChunkWorkItem* work = new ChunkWorkItem(x, z, CHUNK_WORKER_CMD::ALLOC);
            std::unique_lock lock(m_WorkerLock);
            m_WorkItems.push(work);
            m_WorkerCV.notify_one();
            lock.unlock();
        }
    }

}


/*
    We can probably reduce this overhead  by binding the texture once in the renderer
*/
void ChunkManager::AddChunkToRenderer(Chunk *chunk)
{
    auto renderObj = chunk->GetRenderObject();
    renderObj->m_TexturedObject.AddDiffuseMap(m_TextureAtlasDiffuse);
    renderObj->m_TexturedObject.AddSpecularMap(m_TextureAtlasSpecular);
    renderObj->m_TexturedObject.Shininess = 64.0f;
    renderer.AddRenderObject(renderObj);
}


glm::vec2 ChunkManager::CurrentChunkToVec2()
{
    return glm::vec2(static_cast<float>(m_CurrentChunk.first), static_cast<float>(m_CurrentChunk.second));
}

float ChunkManager::DistanceFromCurrentChunk(Chunk* chunk)
{
    return glm::distance(chunk->GetPosition(), CurrentChunkToVec2());
}

float ChunkManager::DistanceFromCurrentChunk(int x, int z)
{
    return glm::distance(glm::vec2(static_cast<float>(x), static_cast<float>(z)), CurrentChunkToVec2());
}


void ChunkManager::ChunkWorkerThread()
{

    //logger.Log(LOGTYPE::INFO, "ChunkManager::ChunkWorkerThread()");
    while(1)
    {
        std::unique_lock lock(m_WorkerLock);

        m_WorkerCV.wait(lock, []{return m_WorkItems.size() > 0;});
        auto work = m_WorkItems.front();
        m_WorkItems.pop();
        lock.unlock();

       // while(1);
        switch(work->cmd)
        {
            case CHUNK_WORKER_CMD::FREE: // we will want to introduce logic to save changes to chunks here eventually
            {
                int before = util::GetMemoryUsageKb();

                /*
                    Add the chunk to the delete list so we can remove it from the used list
                */
                {
                    std::unique_lock tdLock(m_ToDeleteLock);
                    m_ToDeleteList.push_back(work->chunk->GetPositionAsString());
                }
                
                delete work->chunk;
                delete_count++;
                //printf("difference free %ld 0x%x\n", int(util::GetMemoryUsageKb())-before, work->chunk);
                break;
            }
            case CHUNK_WORKER_CMD::ALLOC: // we will want to introduce logic to load changed chunks here eventually
            {
                init_count++;
                if (CAN_ALLOC_CHUNK &&  DistanceFromCurrentChunk(work->x, work->z) < CHUNK_DISTANCE)
                {
                    auto chunk = new Chunk(work->x, work->z, m_ChunkShader, true); 
                    std::unique_lock flock(m_FinishedItemsLock);
                    m_FinishedWork.push(chunk);
                }
                
                //lock.unlock();
                break;
            }
            case CHUNK_WORKER_CMD::UPDATE:
            {
                
                break;
            }
            default: break;
                //logger.Log(LOGTYPE::ERROR, "ChunkManager::ChunkWorkerThread() --> Bad CHUNK_WORKER_CMD");      
        }
        delete work;
    }
}


ChunkManager::~ChunkManager()
{
}

void ChunkManager::PerFrame()
{   
    auto pos = gl.GetCamera()->GetPosition();
    auto xz_pos = glm::vec2(pos.x, pos.z);

    auto diff_x = xz_pos.x - m_CurrentChunk.first*CHUNK_WIDTH;
    auto diff_z = xz_pos.y - m_CurrentChunk.second*CHUNK_WIDTH;

    bool new_chunk = true;

    if (diff_x >= 0.0f && diff_x <= 16.0f && diff_z >= 0.0f && diff_z <= 16.0f)
        new_chunk = false;
    if (new_chunk)
    {
        if (diff_x < 0.0f)
        {
            m_CurrentChunk.first--;
            MapMoveLeft();
        }
        else if (diff_x > 16.0f)
        {
            m_CurrentChunk.first++;
            MapMoveRight();
        }
        else if (diff_z > 16.0f)
        {
            m_CurrentChunk.second++;
            MapMoveForward();
        }
        else if (diff_z < 0.0f)
        {
            m_CurrentChunk.second--;
            MapMoveBackward();
        }

        //logger.Log(LOGTYPE::INFO, "ChunkManager::PerFrame() --> current chunk " + std::to_string(m_CurrentChunk.first) + "," +\
        std::to_string(m_CurrentChunk.second));
    }

    {
        std::unique_lock tdLock(m_ToDeleteLock);
        for (auto& td: m_ToDeleteList)
        {
            m_UsedChunks.erase(td);
            printf("erasing %s\n", td.c_str());
        }
        m_ToDeleteList.clear();
    }




    std::unique_lock lock(m_FinishedItemsLock);
    while(m_FinishedWork.size())
    {
        auto chunk = m_FinishedWork.front();
        m_FinishedWork.pop();
        
        if (m_ActiveChunks.size() >= MAX_CHUNKS)
        { 
            logger.Log(LOGTYPE::WARNING, "ChunkManager::PerFrame() --> reached max chunks=" + std::to_string(MAX_CHUNKS) + " Discarding newly generated chunk.");
            delete chunk;
            CleanFarChunks(1.6f);
            break;
        }
        else
        {
            printf("MAX_CHUNKS %ld\n", MAX_CHUNKS);
        }
        chunk->GenerateChunkMesh(m_ChunkShader); // must do this here as it didnt get done earier
        m_ActiveChunks.push_back(chunk);

        //if (m_UsedChunks.count(chunk->GetPositionAsString()) > 1)
        //    while(1){printf("COPY! %s\n", chunk->GetPositionAsString());}
        //logger.Log(LOGTYPE::INFO, "ChunkManager::PerFrame() --> Adding completed chunk to render list");
        AddChunkToRenderer(chunk);
        //printf("chunk %x, chunk->RenderObj %x\n", chunk, chunk->GetRenderObject());
        break;
    }
    lock.unlock();



}

void ChunkManager::CleanFarChunks()
{
    auto it = m_ActiveChunks.begin();

    while(it != m_ActiveChunks.end())
    {
        auto chunk = *it;
        if (DistanceFromCurrentChunk(chunk) > DELETE_DISTANCE)
        {
            it = m_ActiveChunks.erase(it);
            chunk->GetRenderObject()->m_bDelete = true;
            ChunkWorkItem* work = new ChunkWorkItem(chunk, CHUNK_WORKER_CMD::FREE);
            std::unique_lock lock(m_WorkerLock);
            m_WorkItems.push(work);
            m_WorkerCV.notify_one();
            lock.unlock();
        }
        else
        {
            it++;
        }
    }
}

void ChunkManager::CleanFarChunks(float div)
{
    auto it = m_ActiveChunks.begin();

    while(it != m_ActiveChunks.end())
    {
        auto chunk = *it;
        if (DistanceFromCurrentChunk(chunk) > DELETE_DISTANCE/div)
        {
            it = m_ActiveChunks.erase(it);
            chunk->GetRenderObject()->m_bDelete = true;
            ChunkWorkItem* work = new ChunkWorkItem(chunk, CHUNK_WORKER_CMD::FREE);
            std::unique_lock lock(m_WorkerLock);
            m_WorkItems.push(work);
            m_WorkerCV.notify_one();
            lock.unlock();
        }
        else
        {
            it++;
        }
    }
}


// z++
void ChunkManager::MapMoveForward()
{
    // All items at the bottom the map are invalid
    CleanFarChunks();
    

    for (int x = m_CurrentChunk.first-CHUNK_DISTANCE; x < m_CurrentChunk.first+CHUNK_DISTANCE; x++)
    {
        for (int z = m_CurrentChunk.second; z < m_CurrentChunk.second+CHUNK_DISTANCE; z++)
        {
             bool used = m_UsedChunks.count(pair2String(x,z)) > 0 || !CAN_ALLOC_CHUNK;
            if (used) continue;


            if (DistanceFromCurrentChunk(x, z) < CHUNK_DISTANCE)
            {
                ChunkWorkItem* work = new ChunkWorkItem(x, z, CHUNK_WORKER_CMD::ALLOC);
                m_UsedChunks.insert(pair2String(x, z));
                std::unique_lock lock(m_WorkerLock);
                m_WorkItems.push(work);
                m_WorkerCV.notify_one();
                lock.unlock();
            }
        }
    }
}

//z--
void ChunkManager::MapMoveBackward()
{
     // All items at the bottom the map are invalid
    CleanFarChunks();

    for (int x = m_CurrentChunk.first-CHUNK_DISTANCE; x < m_CurrentChunk.first+CHUNK_DISTANCE; x++)
    {
        for (int z = m_CurrentChunk.second-CHUNK_DISTANCE; z < m_CurrentChunk.second; z++)
        {
            bool used = m_UsedChunks.count(pair2String(x,z)) > 0 || !CAN_ALLOC_CHUNK;
            if (used) continue;
            if (DistanceFromCurrentChunk(x, z)  < CHUNK_DISTANCE)
            {
                ChunkWorkItem* work = new ChunkWorkItem(x, z, CHUNK_WORKER_CMD::ALLOC);
                 m_UsedChunks.insert(pair2String(x, z));
                std::unique_lock lock(m_WorkerLock);
                m_WorkItems.push(work);
                m_WorkerCV.notify_one();
                lock.unlock();
            }
        }
    }
}

// x--
void ChunkManager::MapMoveLeft()
{
        // All items at the bottom the map are invalid
    CleanFarChunks();

    for (int x = m_CurrentChunk.first-CHUNK_DISTANCE; x < m_CurrentChunk.first; x++)
    {
        for (int z = m_CurrentChunk.second-CHUNK_DISTANCE; z < m_CurrentChunk.second+CHUNK_DISTANCE; z++)
        {
             bool used = m_UsedChunks.count(pair2String(x,z)) > 0 || !CAN_ALLOC_CHUNK;
            if (used) continue;
            if (glm::distance(glm::vec2(static_cast<float>(m_CurrentChunk.first), static_cast<float>(m_CurrentChunk.second)), \
            glm::vec2(static_cast<float>(x), static_cast<float>(z))) < CHUNK_DISTANCE)
            {
                ChunkWorkItem* work = new ChunkWorkItem(x, z, CHUNK_WORKER_CMD::ALLOC);
                m_UsedChunks.insert(pair2String(x, z));
                std::unique_lock lock(m_WorkerLock);
                m_WorkItems.push(work);
                m_WorkerCV.notify_one();
                lock.unlock();
            }
        }
    }
}

// x++
void ChunkManager::MapMoveRight()
{
    // All items at the bottom the map are invalid
    //std::vector<Chunk*> invalids;
    //printf("map move foreward\n");
    CleanFarChunks();

    for (int x = m_CurrentChunk.first; x < m_CurrentChunk.first+CHUNK_DISTANCE; x++)
    {
        for (int z = m_CurrentChunk.second-CHUNK_DISTANCE; z < m_CurrentChunk.second+CHUNK_DISTANCE; z++)
        {
            bool used = m_UsedChunks.count(pair2String(x,z)) > 0 || !CAN_ALLOC_CHUNK;
            if (used) continue;
            if (DistanceFromCurrentChunk(x, z)  < CHUNK_DISTANCE)
            {
                ChunkWorkItem* work = new ChunkWorkItem(x, z, CHUNK_WORKER_CMD::ALLOC);
                m_UsedChunks.insert(pair2String(x, z));
                std::unique_lock lock(m_WorkerLock);
                m_WorkItems.push(work);
                m_WorkerCV.notify_one();
                lock.unlock();
            }
        }
    }
}

ChunkWorkItem::ChunkWorkItem(Chunk *nchunk, CHUNK_WORKER_CMD wcmd)  : chunk(nchunk), cmd(wcmd)
{
}
ChunkWorkItem::ChunkWorkItem(int xcoord, int zcoord, CHUNK_WORKER_CMD wcmd) : x(xcoord), z(zcoord), cmd(wcmd), chunk(nullptr)
{
};

std::string pair2String(int x, int y)
{
    auto ret = std::to_string(x) + "~" + std::to_string(y);
    return ret;
}


