#include "chunk.h"

extern GLManager gl;
extern Logger logger;
extern Renderer renderer;

int MAX_CHUNKS = 1000;
float CHUNK_DISTANCE = 2.0f;
float DELETE_DISTANCE = 2.0f;

std::condition_variable ChunkManager::m_WorkerCV;
std::mutex ChunkManager::m_WorkerLock;
std::mutex ChunkManager::m_FinishedItemsLock;
std::queue<ChunkWorkItem *> ChunkManager::m_WorkItems;
std::mutex ChunkManager::m_ToDeleteLock;
std::vector<std::string> ChunkManager::m_ToDeleteList;
std::queue<ChunkWorkItem*> ChunkManager::m_FinishedWork;
std::unordered_map<std::string, Chunk *> ChunkManager::m_UsedChunks;
ShaderProgram *ChunkManager::m_ChunkShader;
RenderObject *ChunkManager::m_RenderObj;
GPUAllocator *ChunkManager::m_GPUMemoryManager;
Texture *ChunkManager::m_TextureAtlasDiffuse;
Texture *ChunkManager::m_TextureAtlasSpecular;
VertexArray *ChunkManager::m_VA;
FastNoiseLite ChunkManager::m_ChunkHeightNoise;
FastNoiseLite ChunkManager::m_StructureNoise;
FastNoiseLite ChunkManager::m_BiomeNoise;
std::vector<Chunk *> ChunkManager::m_ActiveChunks;
std::pair<int, int> ChunkManager::m_CurrentChunk;
std::vector<ChunkVertex> ChunkManager::m_stashedVertices;

#define DEFAULT_CHUNK_GROUND 64
#define HEIGHT_SCALE 96.0f // multiplier to adjust terrain
#define SCALE 0.1f

Chunk::Chunk(int x, int z, ShaderProgram *sp) : m_xCoord(x), m_zCoord(z)
{
    GenerateChunk();
    GenerateChunkMesh(sp);
}

Chunk::Chunk(int x, int z, ShaderProgram *sp, bool delay) : m_xCoord(x), m_zCoord(z)
{
    GenerateChunk();

    if (!delay)
        GenerateChunkMesh(sp);
}

Chunk::~Chunk()
{
    // delete m_RenderObj; // renderObj deletes VertexArray deletes (indexBuffer, vertexBuffer)
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

void Chunk::GenerateChunkMesh(ShaderProgram *sp)
{

    // m_IB = new IndexBuffer(m_Indices.data(), m_Indices.size());
    // m_VB = new VertexBuffer((float *)m_Vertices.data(), m_Vertices.size() * sizeof(ChunkVertex));
    //  m_VB = new VertexBuffer(sizeof(ChunkVertex)*24*CHUNK_WIDTH*CHUNK_WIDTH*MAX_CHUNK_HEIGHT);
    //  m_VB->SetData((float *)m_Vertices.data(), m_Vertices.size() * sizeof(ChunkVertex));
    // m_VB->SetLayout(vertex_layout);

    // printf("Chunk::GenerateChunkMesh()\n");

    // printf("here!\n");
    // m_VA = new VertexArray();
    // m_VA->AddVertexBuffer(m_VB);
    // m_VA->AddIndexBuffer(m_IB);
    // m_VA->SetCount(m_Vertices.size());
    // m_RenderObj = new RenderObject(m_VA, m_VB, sp, OBJECTYPE::ChunkMesh);
    // m_RenderObj = new RenderObject(m_VA, m_VB, sp, m_IB, OBJECTYPE::ChunkMesh);
    // m_RenderObj->Translate(glm::vec3(m_xCoord * CHUNK_WIDTH, 0.0f, m_zCoord * CHUNK_WIDTH));
    //// if (m_bHasDiamond)
    //     m_RenderObj->ToggleWireFrame();
    // printf("num vertices %d\n", m_Vertices.size());
    m_Vertices.clear();
    m_Vertices.shrink_to_fit();
    // m_Indices.clear();
    // m_Indices.shrink_to_fit();
}

bool Chunk::isActive(int x, int y, int z)
{
    if (IS_IN_CHUNK(x, y, z))
        if (m_Blocks[x][y][z].isActive())
            return true;
    return false;
}

bool Chunk::SetBlock(int x, int y, int z, BlockType block)
{
     //printf("Chunk::SetBlock()\n");
     printf("Chunk::SetBlock() %d, %d, %d\n", x, y, z);
    if (IS_IN_CHUNK(x, y, z))
    {  
        
        Block& blk = this->m_Blocks[x][y][z];
        printf("Chunk::SetBlock() %d, %d, %d\n", x, y, z);
        //f (blk.getBlockType() == (BlockType)0)
         //   while(1);

        //printf("act %d\n", blk.m_Active_Type);
        //auto act = blk.isActive();
        
        if (true)
        {
            //blk.setActive(true);
            //blk.setType(block);
            printf("Chunk::SetBlock()\n");
            if (IS_IN_CHUNK(x, y+1, z) && !m_Blocks[x][y+1][z].isActive())
                BlockGenVertices(block, x, y, z, BLOCKFACE::TOP);
            if (IS_IN_CHUNK(x, y-1, z) && !m_Blocks[x][y-1][z].isActive())
                BlockGenVertices(block, x, y, z, BLOCKFACE::BOTTOM); 


            BlockGenVertices(block, x, y, z, BLOCKFACE::FRONT); 
            BlockGenVertices(block, x, y, z, BLOCKFACE::BACK); 
            BlockGenVertices(block, x, y, z, BLOCKFACE::LEFT); 
            BlockGenVertices(block, x, y, z, BLOCKFACE::RIGHT);  
            return true;   
        }
    }
    return false;
}

void Chunk::BlockGenVertices(BlockType blocktype, float x, float y, float z, BLOCKFACE face)
{
    //  printf("face: %d ==> %.2f, %.2f, %.2f\n", face,x, y, z);
    auto type = blocktype;
    // auto faceIndices = std::vector<unsigned int>{0, 1, 2, 1, 3, 2};
    // for (int i = 0; i < faceIndices.size(); i++)
    //    faceIndices[i] += m_Vertices.size();
    // m_Indices.insert(m_Indices.end(), faceIndices.begin(), faceIndices.end());

    std::vector<ChunkVertex> vertvec = GetFace(face, blocktype, x + m_xCoord * CHUNK_WIDTH, y, z + m_zCoord * CHUNK_WIDTH);
    m_Vertices.insert(m_Vertices.end(), vertvec.begin(), vertvec.end());
}

void Chunk::GenerateChunk()
{

    std::vector<Block *> actives;
    std::vector<std::vector<int>> activeCoords;
    float biomeNoise = ChunkManager::m_BiomeNoise.GetNoise(static_cast<float>(m_xCoord * CHUNK_WIDTH), static_cast<float>(m_zCoord * CHUNK_WIDTH));
    // printf("%.2f\n", biomeNoise);
    BIOMETYPE biome = Chunk::BiomeSelect(biomeNoise);
    bool hasActive = false;

    // Initial Generation
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {

            float noise = ChunkManager::m_ChunkHeightNoise.GetNoise(static_cast<float>(x + m_xCoord * CHUNK_WIDTH), static_cast<float>(z + m_zCoord * CHUNK_WIDTH));
            int surface = static_cast<int>(DEFAULT_CHUNK_GROUND + (noise * BIOME::GetSurfaceVariation(biome)));
            for (int y = 0; y < surface; y++)
            {
                auto &block = m_Blocks[x][y][z];
                if (block.getBlockType() == BlockType::BLOCKNONE)
                    block.setType(GetBlockType(x, y, z, surface - 1, biome));
                bool doOrePass = util::Random() > ORE_PASS_THRESHOLD;
                if (x == 0 || z == 0 || x == 15 || z == 15 || y == 0 || y == (surface - 1))
                {
                    // if (y == surface - 1 && util::Random() < 0.002)
                    //{
                    //      AddStructure(this, x, y, z, STRUCTURETYPE::tree1, actives, activeCoords);
                    //      printf("adding tree %.2f, %.2f\n", static_cast<float>(x + m_xCoord * CHUNK_WIDTH), static_cast<float>(z + m_zCoord * CHUNK_WIDTH));
                    // }

                    block.setActive(true);
                }

                if (doOrePass)
                    OrePopulatePass({x, y, z}, this);
                if (block.isActive())
                {
                    hasActive = true;
                    actives.push_back(&block);
                    activeCoords.push_back({x, y, z, y == (surface - 1)});
                }
            }
        }
    }

    for (int i = 0; i < actives.size(); i++)
    {
        std::vector<BLOCKFACE> faces;
        auto &block = actives[i];
        auto &coords = activeCoords[i];

        auto &x = coords[0];
        auto &y = coords[1];
        auto &z = coords[2];
        auto &isSurface = coords[3];

        if (isSurface == 1)
        {
            faces.push_back(BLOCKFACE::TOP);
            if (!isActive(x - 1, y, z) || !IS_IN_CHUNK(x - 1, y, z))
                faces.push_back(BLOCKFACE::LEFT);
            if (!isActive(x + 1, y, z) || !IS_IN_CHUNK(x + 1, y, z))
                faces.push_back(BLOCKFACE::RIGHT);
            if (!isActive(x, y - 1, z) || !IS_IN_CHUNK(x, y - 1, z))
                faces.push_back(BLOCKFACE::BOTTOM);
            if (!isActive(x, y, z - 1) || !IS_IN_CHUNK(x, y, z - 1))
                faces.push_back(BLOCKFACE::BACK);
            if (!isActive(x, y, z + 1) || !IS_IN_CHUNK(x, y, z + 1))
                faces.push_back(BLOCKFACE::FRONT);
        }
        else
        {
            if (x == 0)
                faces.push_back(BLOCKFACE::LEFT);
            if (x == 15)
                faces.push_back(BLOCKFACE::RIGHT);
            if (z == 0)
                faces.push_back(BLOCKFACE::BACK);
            if (z == 15)
                faces.push_back(BLOCKFACE::FRONT);
            if (y == 0)
                faces.push_back(BLOCKFACE::BOTTOM);
        }

        for (auto &face : faces)
            BlockGenVertices(block->getBlockType(), x, y, z, face);
    }
}

void Chunk::OrePopulatePass(std::vector<int> coordStart, Chunk *chunk)
{
    float noise = util::Random();
    int y = coordStart[1];
    if (ORE_CAN_GEN_IRON(noise, y))
    {
        OrePassFill(coordStart, BlockType::Iron, chunk);
    }
    else if (ORE_CAN_GEN_GOLD(noise, y))
    {
        OrePassFill(coordStart, BlockType::Gold, chunk);
    }
    else if (ORE_CAN_GEN_COAL(noise, y))
    {
        OrePassFill(coordStart, BlockType::Coal, chunk);
    }
    else if (ORE_CAN_GEN_DIAMOND(noise, y))
    {
        OrePassFill(coordStart, BlockType::Diamond, chunk);
    }
    else if (ORE_CAN_GEN_STONE(noise, y))
        OrePassFill(coordStart, BlockType::Stone, chunk);
}

void Chunk::OrePassFill(std::vector<int> coordStart, BlockType ore, Chunk *chunk)
{
    int veinSize = static_cast<int>(util::RandomMax(OreGetVeinSize(ore)));
    std::vector<std::vector<int>> dirs;
    int x = coordStart[0];
    int y = coordStart[1];
    int z = coordStart[2];

    for (int i = 0; i < veinSize; i++)
    {
        auto dir = OreDirections[util::RandomMax((OreDirections.size() - 1))];
        auto &nx = dir[0];
        auto &ny = dir[1];
        auto &nz = dir[2];
        if (IS_IN_CHUNK(x + nx, y + ny, z + nz))
        {
            x += nx;
            y += ny;
            z += nz;

            auto &block = chunk->m_Blocks[x][y][z];
            block.setType(ore);
        }
    }
}

void Chunk::AddStructure(Chunk *chunk, int x, int y, int z, STRUCTURETYPE structure, std::vector<Block *> &actives, std::vector<std::vector<int>> &activeCoords)
{
    switch (structure)
    {
    case STRUCTURETYPE::tree1:
    {
        for (auto &sc : Schematic_tree1)
        {
            auto nx = x + sc.x;
            auto ny = y + sc.y;
            auto nz = z + sc.z;
            if (IS_IN_CHUNK(nx, ny, nz))
            {
                auto &block = chunk->m_Blocks[nx][ny][nz];
                block.setActive(true);
                block.setType(sc.block);
                actives.push_back(&block);
                activeCoords.push_back({nx, ny, nz});
            }
        }
    }
    default:
        break;
    }
}

int Chunk::OreGetVeinSize(BlockType ore)
{
    switch (ore)
    {
    case BlockType::Iron:
        return ORE_IRON_MAX_VEIN;
    case BlockType::Gold:
        return ORE_GOLD_MAX_VEIN;
    case BlockType::Coal:
        return ORE_COAL_MAX_VEIN;
    case BlockType::Diamond:
        return ORE_DIAMOND_MAX_VEIN;
    case BlockType::Stone:
        return ORE_STONE_MAX_VEIN;
    default:
        return 0;
    }
}

BIOMETYPE Chunk::BiomeSelect(float biomeNoise)
{
    if (biomeNoise < -0.5f)
        return BIOMETYPE::HILLS;
    else if (biomeNoise < 1.2f)
        return BIOMETYPE::Desert;
    else
        return BIOMETYPE::PLAINS;
}

BlockType Chunk::GetBlockType(int x, int y, int z, int surface, BIOMETYPE biome)
{
    switch (biome)
    {
    case BIOMETYPE::HILLS:
        return BIOME::Hills_GetBlockType(x, y, z, surface);
    case BIOMETYPE::PLAINS:
        return BIOME::Plains_GetBlockType(x, y, z, surface);
    case BIOMETYPE::EXTREME_HILLS:
        return BIOME::ExtremeHills_GetBlockType(x, y, z, surface);
    case BIOMETYPE::Desert:
        return BIOME::Desert_GetBlockType(x, y, z, surface);
    default:
    {
        logger.Log(LOGTYPE::ERROR, "Chunk::GetBlockType() --> Got invalid BIOME.");
        return BlockType::Dirt;
    }
    }
}

ChunkManager::ChunkManager()
{
    m_GPUMemoryManager = new GPUAllocator(0.5f, sizeof(ChunkVertex) * 3);
    m_CurrentChunk = {0, 0};
    auto VB = m_GPUMemoryManager->GetVertexBuffer();
    BufferLayout *vertex_layout = new BufferLayout({new BufferElement("COORDS", ShaderDataType::Float3, false),
                                                    new BufferElement("faceBlockType", ShaderDataType::Int, false),
                                                    new BufferElement("texCoord", ShaderDataType::Float2, false)});
    VB->SetLayout(vertex_layout);

    m_VA = new VertexArray();
    ChunkManager::m_VA->SetCount(ChunkManager::m_GPUMemoryManager->m_AllocatorCapacity / sizeof(ChunkVertex)); //
    m_VA->AddVertexBuffer(VB);
    // m_VA->SetCount(m_GPUMemoryManager->m_AllocatorCapacity/sizeof(ChunkVertex));

    m_ChunkHeightNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_ChunkHeightNoise.SetSeed(DEFAULT_NOISE_SEED);
    m_BiomeNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_BiomeNoise.SetFrequency(0.0003f);
    m_BiomeNoise.SetSeed(DEFAULT_NOISE_SEED);
    m_StructureNoise.SetNoiseType(FastNoiseLite::NoiseType_Value);
    m_StructureNoise.SetFrequency(0.01f);
    m_StructureNoise.SetSeed(DEFAULT_NOISE_SEED);

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
        while (1)
            ;
    }
    m_TextureAtlasSpecular = new Texture("/home/cole/Documents/voxel-engine/src/textures/texture_atlas.png", "spec");
    m_TextureAtlasDiffuse = new Texture("/home/cole/Documents/voxel-engine/src/textures/texture_atlas.png", "diff");

    m_RenderObj = new RenderObject(m_VA, VB, m_ChunkShader, OBJECTYPE::ChunkMesh);
    m_RenderObj->m_TexturedObject.AddDiffuseMap(m_TextureAtlasDiffuse);
    m_RenderObj->m_TexturedObject.AddSpecularMap(m_TextureAtlasSpecular);

    renderer.AddRenderObject(m_RenderObj);
    // can see CHUNK_DISTANCE each way
    // auto chunk = new Chunk(0, 0, m_ChunkShader);
    // AddChunkToRenderer(chunk);
    // m_ActiveChunks.push_back(chunk);
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

float ChunkManager::DistanceFromCurrentChunk(Chunk *chunk)
{
    return glm::distance(chunk->GetPosition(), CurrentChunkToVec2());
}

float ChunkManager::DistanceFromCurrentChunk(int x, int z)
{
    return glm::distance(glm::vec2(static_cast<float>(x), static_cast<float>(z)), CurrentChunkToVec2());
}

void ChunkManager::ChunkWorkerThread()
{

    // logger.Log(LOGTYPE::INFO, "ChunkManager::ChunkWorkerThread()");
    while (1)
    {
        std::unique_lock lock(m_WorkerLock);

        m_WorkerCV.wait(lock, []{ return m_WorkItems.size() > 0; });
        auto work = m_WorkItems.front();
        m_WorkItems.pop();
        lock.unlock();
        // while(1);
        switch (work->cmd)
        {
            case CHUNK_WORKER_CMD::FREE: // we will want to introduce logic to save changes to chunks here eventually
            {
                
                /*
                    Add the chunk to the delete list so we can remove it from the used list
                */

                std::unique_lock tdLock(m_ToDeleteLock);
                m_ToDeleteList.push_back(work->chunk->GetPositionAsString());

                delete work->chunk;
                delete work;
                // printf("difference free %ld 0x%x\n", int(util::GetMemoryUsageKb())-before, work->chunk);
                break;
            }
            case CHUNK_WORKER_CMD::ALLOC: // we will want to introduce logic to load changed chunks here eventually
            {
                if (CAN_ALLOC_CHUNK && DistanceFromCurrentChunk(work->x, work->z) < CHUNK_DISTANCE)
                {
                    auto chunk = new Chunk(work->x, work->z, m_ChunkShader, true);
                    std::unique_lock flock(m_FinishedItemsLock);
                    work->chunk = chunk;
                    m_FinishedWork.push(work);
                }
                else
                {
                    std::unique_lock tdLock(m_ToDeleteLock);
                    m_ToDeleteList.push_back(pair2String(work->x, work->z));
                }

                // lock.unlock();
                break;
            }
            case CHUNK_WORKER_CMD::UPDATE:
            {
                
                std::unique_lock flock(m_FinishedItemsLock);
                //auto& update = work->update;
                //auto chunk = work->chunk;
                m_FinishedWork.push(work);
                break;
            }
            case CHUNK_WORKER_CMD::STRUCTURE_ADD:
            {
                // float biomeNoise = m_BiomeNoise.GetNoise(static_cast<float>(work->x), static_cast<float>(work->z));
                // auto biome = Chunk::BiomeSelect(biomeNoise);
                // float structureNoise = m_StructureNoise.GetNoise(static_cast<float>(work->x), static_cast<float>(work->z));
                // auto structureType = BIOME::GetStructure(biome, structureNoise);

                break;
            }
            default:
            {
                
                break;
            }
            // logger.Log(LOGTYPE::ERROR, "ChunkManager::ChunkWorkerThread() --> Bad CHUNK_WORKER_CMD");
        }
        //delete work;
    }
}

ChunkManager::~ChunkManager()
{
}

void ChunkManager::PerFrame()
{
    auto pos = gl.GetCamera()->GetPosition();
    auto xz_pos = glm::vec2(pos.x, pos.z);

    auto diff_x = xz_pos.x - m_CurrentChunk.first * CHUNK_WIDTH;
    auto diff_z = xz_pos.y - m_CurrentChunk.second * CHUNK_WIDTH;

    bool new_chunk = true;

    if (diff_x >= 0.0f && diff_x <= 16.0f && diff_z >= 0.0f && diff_z <= 16.0f)
        new_chunk = false;
    if (new_chunk)
    {
        if (diff_x < 0.0f)
            m_CurrentChunk.first--;
        if (diff_x > 16.0f)
            m_CurrentChunk.first++;
        if (diff_z > 16.0f)
            m_CurrentChunk.second++;
        if (diff_z < 0.0f)
            m_CurrentChunk.second--;
        //logger.Log(LOGTYPE::INFO, "ChunkManager::PerFrame() --> current chunk " + std::to_string(m_CurrentChunk.first) + "," +\
        std::to_string(m_CurrentChunk.second));
    }
    MapMove();

    {
        std::unique_lock tdLock(m_ToDeleteLock);
        for (auto &td : m_ToDeleteList)
        {
            m_GPUMemoryManager->FreeData(td);
            m_UsedChunks.erase(td);
        }
        m_ToDeleteList.clear();
    }

    std::unique_lock lock(m_FinishedItemsLock);
    while (m_FinishedWork.size())
    {
        if (m_ActiveChunks.size() >= MAX_CHUNKS)
        {
            logger.Log(LOGTYPE::WARNING, "ChunkManager::PerFrame() --> reached max chunks=" + std::to_string(MAX_CHUNKS) + " Discarding newly generated chunk.");
            // CleanFarChunks(1.6f);
            break;
        }
        auto item = m_FinishedWork.front();
        auto chunk = item->chunk;
        m_FinishedWork.pop();

        switch(item->cmd)
        {
            case CHUNK_WORKER_CMD::ALLOC:
            {
                if (!ChunkManager::m_GPUMemoryManager->PutData(chunk->GetPositionAsString(), chunk->m_Vertices.data(), chunk->m_Vertices.size() * sizeof(ChunkVertex)))
                {
                    chunk->GenerateChunkMesh(m_ChunkShader); // must do this here as it didnt get done
                    delete chunk;
                    break;
                }
                chunk->GenerateChunkMesh(m_ChunkShader); // must do this here as it didnt get done
                m_UsedChunks[chunk->GetPositionAsString()] = chunk;
                m_ActiveChunks.push_back(chunk);
                break;
            }
            case CHUNK_WORKER_CMD::UPDATE:
            {
                auto it = m_UsedChunks.find(pair2String(item->x, item->z));
                if (it == m_UsedChunks.end()) // a new chunk, not an update
                {
                    break;
                }

                auto chunk = it->second;
                if (chunk == nullptr)
                {
                    break;
                }

               


                auto& update = item->update;
                if(!chunk->SetBlock(update.x, update.y, update.z, update.type))
                {
                    printf("failed set block\n");
                    break;
                }
                if (!ChunkManager::m_GPUMemoryManager->PutData(chunk->GetPositionAsString(), chunk->m_Vertices.data(), chunk->m_Vertices.size() * sizeof(ChunkVertex)))
                {
                    chunk->GenerateChunkMesh(m_ChunkShader); // must do this here as it didnt get done
                    break;
                }
                chunk->GenerateChunkMesh(m_ChunkShader); // must do this here as it didnt get done earier
                break;
            }
            default:
                break;
        }
        delete item;

        // m_stashedVertices.insert(m_stashedVertices.begin(), chunk->m_Vertices.begin(), chunk->m_Vertices.end());

        /*
            Do this if GPU buffer cannot find a valid slot for the data
        */
        // printf("here! %lld\n", ChunkManager::m_VA->GetCount() + m_Vertices.size());
        //ChunkManager::m_VA->AddVertexBuffer(ChunkManager::m_GPUMemoryManager->GetVertexBuffer());

        // m_stashedVertices.clear();
        // m_stashedVertices.shrink_to_fit();

        // clear
        

        /*
            We must update the UsedChunks entry here, as we only entered it early as UsedChunks[pos] = nullptr earlier

            When accessing from UsedChunks we must ensure that the retrieved pointer is not a nullptr
        */

       

        break;
    }

    // printf("PerFrame() done\n");
}

void ChunkManager::PlaceBlock(std::pair<int, int> &chunkCoord, int x, int y, int z, BlockType block)
{
    auto it = m_UsedChunks.find(pair2String(chunkCoord.first, chunkCoord.second));
    BlockUpdate update(x, y, z, block);
    auto work = new ChunkWorkItem(chunkCoord.first, chunkCoord.second, CHUNK_WORKER_CMD::UPDATE, update);   
    std::unique_lock lock(m_WorkerLock);
    printf("push\n");
    m_WorkItems.push(work);
    m_WorkerCV.notify_one();
    //lock.unlock();
}

void ChunkManager::CleanFarChunks()
{
    auto it = m_ActiveChunks.begin();

    while (it != m_ActiveChunks.end())
    {
        auto chunk = *it;
        if (DistanceFromCurrentChunk(chunk) > DELETE_DISTANCE)
        {
            it = m_ActiveChunks.erase(it);
            // chunk->GetRenderObject()->m_bDelete = true;
            ChunkWorkItem *work = new ChunkWorkItem(chunk, CHUNK_WORKER_CMD::FREE);
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

    while (it != m_ActiveChunks.end())
    {
        auto chunk = *it;
        if (DistanceFromCurrentChunk(chunk) > DELETE_DISTANCE / div)
        {
            it = m_ActiveChunks.erase(it);
            chunk->GetRenderObject()->m_bDelete = true;
            ChunkWorkItem *work = new ChunkWorkItem(chunk, CHUNK_WORKER_CMD::FREE);
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

void ChunkManager::MapMove()
{

    // All items at the bottom the map are invalid
    CleanFarChunks();

    std::vector<ChunkWorkItem *> workItems;
    for (int x = 0; x <= CHUNK_DISTANCE; x++)
    {

        // /if (!CAN_ALLOC_CHUNK || CHUNK_WORKER_QUEUE_FULL) printf("break\n");break;
        auto px = m_CurrentChunk.first + x;
        auto nx = m_CurrentChunk.first - x;
        for (int z = 0; z <= CHUNK_DISTANCE; z++)
        {
            if (!CAN_ALLOC_CHUNK || CHUNK_WORKER_QUEUE_FULL)
            {
                break;
            }

            auto pz = m_CurrentChunk.second + z;
            auto nz = m_CurrentChunk.second - z;
            std::vector<std::vector<int>> locations = {{px, pz}, {nx, pz}, {px, nz}, {nx, nz}};
            for (auto &pos : locations)
            {

                auto cx = pos[0];
                auto cz = pos[1];
                auto dist = DistanceFromCurrentChunk(cx, cz);
                if (!CAN_ALLOC_CHUNK || CHUNK_WORKER_QUEUE_FULL)
                {
                    break;
                }

                auto key = pair2String(cx, cz);

                // if (dist < STRUCTURES_GEN_DISTANCE)
                //{
                //     m_StructureNoise.GetNoise(static_cast<float>(cx), static_cast<float>(cz));
                //     if (m_UsedChunks.find(key) != m_UsedChunks.end())
                //     {
                //         ChunkWorkItem *work = new ChunkWorkItem(cx, cz, CHUNK_WORKER_CMD::STRUCTURE_ADD);
                //         workItems.push_back(work);
                //     }
                // }

                bool used = m_UsedChunks.find(key) != m_UsedChunks.end();
                if (used)
                    continue;

                if (dist < CHUNK_DISTANCE)
                {
                    ChunkWorkItem *work = new ChunkWorkItem(cx, cz, CHUNK_WORKER_CMD::ALLOC);
                    m_UsedChunks.insert({key, nullptr}); // will update this when chunk is finished processing in PerFrame()
                    workItems.push_back(work);           // save for post-processing
                }
            }
        }
    }
    // if we could offload this sorting to the worker threads and a single work item that would be good
    std::sort(workItems.begin(), workItems.end(), [](ChunkWorkItem *a, ChunkWorkItem *b)
              {
        float distA = DistanceFromCurrentChunk(a->x, a->z);
        float distB = DistanceFromCurrentChunk(b->x, b->z);
        return distA < distB; });

    for (auto &work : workItems)
    {
        std::unique_lock lock(m_WorkerLock);
        m_WorkItems.push(work);
        m_WorkerCV.notify_one();
        lock.unlock();
    }
}

ChunkWorkItem::ChunkWorkItem(Chunk *nchunk, CHUNK_WORKER_CMD wcmd) : chunk(nchunk), cmd(wcmd)
{
}

ChunkWorkItem::ChunkWorkItem(int xcoord, int zcoord, CHUNK_WORKER_CMD wcmd, BlockUpdate &update):  x(xcoord), z(zcoord), cmd(wcmd), update(update)
{

}

ChunkWorkItem::ChunkWorkItem(int xcoord, int zcoord, CHUNK_WORKER_CMD wcmd) : x(xcoord), z(zcoord), cmd(wcmd), chunk(nullptr)
{
}


std::string ChunkWorkItem::GetPositionAsString()
{
    return pair2String(x, z);
}

std::string pair2String(int x, int y)
{
    auto ret = std::to_string(x) + "~" + std::to_string(y);
    return ret;
}

std::vector<ChunkVertex> GetFrontFace(float x, float y, float z, BlockType type)
{
    auto tex_front = Block::GenBlockVertices(type, BLOCKFACE::FRONT);
    return std::vector<ChunkVertex>{
        // Front Face
        ChunkVertex{{x + -0.5f, y + -0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), {tex_front[0].first, tex_front[0].second}}, // Bottom-left
        ChunkVertex{{x + 0.5f, y + -0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), {tex_front[1].first, tex_front[1].second}},  // Bottom-right
        ChunkVertex{{x + -0.5f, y + 0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), {tex_front[2].first, tex_front[2].second}},  // Top-left
        ChunkVertex{{x + -0.5f, y + 0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), {tex_front[2].first, tex_front[2].second}},  // Top-left
        ChunkVertex{{x + 0.5f, y + 0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), {tex_front[3].first, tex_front[3].second}},   // Top-right
        ChunkVertex{{x + 0.5f, y + -0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), {tex_front[1].first, tex_front[1].second}},  // Bottom-right
    };
}

std::vector<ChunkVertex> GetBackFace(float x, float y, float z, BlockType type)
{
    auto tex_back = Block::GenBlockVertices(type, BLOCKFACE::BACK);
    return std::vector<ChunkVertex>{
        // Back Face
        ChunkVertex{{x + -0.5f, y + -0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), {tex_back[0].first, tex_back[0].second}}, // Bottom-left
        ChunkVertex{{x + 0.5f, y + -0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), {tex_back[1].first, tex_back[1].second}},  // Bottom-right
        ChunkVertex{{x + -0.5f, y + 0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), {tex_back[2].first, tex_back[2].second}},  // Top-left
        ChunkVertex{{x + -0.5f, y + 0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), {tex_back[2].first, tex_back[2].second}},  // Top-left
        ChunkVertex{{x + 0.5f, y + 0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), {tex_back[3].first, tex_back[3].second}},   // Top-right
        ChunkVertex{{x + 0.5f, y + -0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), {tex_back[1].first, tex_back[1].second}},  // Bottom-right
    };
}

std::vector<ChunkVertex> GetLeftFace(float x, float y, float z, BlockType type)
{
    auto tex_left = Block::GenBlockVertices(type, BLOCKFACE::LEFT);
    return std::vector<ChunkVertex>{
        // Left Face
        ChunkVertex{{x + -0.5f, y + -0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), {tex_left[0].first, tex_left[0].second}}, // Bottom-left
        ChunkVertex{{x + -0.5f, y + -0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), {tex_left[1].first, tex_left[1].second}},  // Bottom-right
        ChunkVertex{{x + -0.5f, y + 0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), {tex_left[2].first, tex_left[2].second}},  // Top-left
        ChunkVertex{{x + -0.5f, y + 0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), {tex_left[2].first, tex_left[2].second}},  // Top-left
        ChunkVertex{{x + -0.5f, y + 0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), {tex_left[3].first, tex_left[3].second}},   // Top-right
        ChunkVertex{{x + -0.5f, y + -0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), {tex_left[1].first, tex_left[1].second}},  // Bottom-right
    };
}

std::vector<ChunkVertex> GetRightFace(float x, float y, float z, BlockType type)
{
    auto tex_right = Block::GenBlockVertices(type, BLOCKFACE::RIGHT);
    return std::vector<ChunkVertex>{
        // Right Face
        ChunkVertex{{x + 0.5f, y + -0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), {tex_right[0].first, tex_right[0].second}}, // Bottom-left
        ChunkVertex{{x + 0.5f, y + -0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), {tex_right[1].first, tex_right[1].second}},  // Bottom-right
        ChunkVertex{{x + 0.5f, y + 0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), {tex_right[2].first, tex_right[2].second}},  // Top-left
        ChunkVertex{{x + 0.5f, y + 0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), {tex_right[2].first, tex_right[2].second}},  // Top-left
        ChunkVertex{{x + 0.5f, y + 0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), {tex_right[3].first, tex_right[3].second}},   // Top-right
        ChunkVertex{{x + 0.5f, y + -0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), {tex_right[1].first, tex_right[1].second}},  // Bottom-right
    };
}

std::vector<ChunkVertex> GetTopFace(float x, float y, float z, BlockType type)
{
    auto tex_top = Block::GenBlockVertices(type, BLOCKFACE::TOP);
    return std::vector<ChunkVertex>{
        // Top Face
        ChunkVertex{{x + -0.5f, y + 0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), {tex_top[0].first, tex_top[0].second}}, // Bottom-left
        ChunkVertex{{x + 0.5f, y + 0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), {tex_top[1].first, tex_top[1].second}},  // Bottom-right
        ChunkVertex{{x + -0.5f, y + 0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), {tex_top[2].first, tex_top[2].second}},  // Top-left
        ChunkVertex{{x + -0.5f, y + 0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), {tex_top[2].first, tex_top[2].second}},  // Top-left
        ChunkVertex{{x + 0.5f, y + 0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), {tex_top[3].first, tex_top[3].second}},   // Top-right
        ChunkVertex{{x + 0.5f, y + 0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), {tex_top[1].first, tex_top[1].second}},  // Bottom-right
    };
}

std::vector<ChunkVertex> GetBottomFace(float x, float y, float z, BlockType type)
{
    auto tex_bottom = Block::GenBlockVertices(type, BLOCKFACE::BOTTOM);
    return std::vector<ChunkVertex>{
        // Bottom Face
        ChunkVertex{{x + -0.5f, y + -0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), {tex_bottom[0].first, tex_bottom[0].second}}, // Bottom-left
        ChunkVertex{{x + 0.5f, y + -0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), {tex_bottom[1].first, tex_bottom[1].second}},  // Bottom-right
        ChunkVertex{{x + -0.5f, y + -0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), {tex_bottom[2].first, tex_bottom[2].second}},  // Top-left
        ChunkVertex{{x + -0.5f, y + -0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), {tex_bottom[2].first, tex_bottom[2].second}},  // Top-left
        ChunkVertex{{x + 0.5f, y + -0.5f, z + 0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), {tex_bottom[3].first, tex_bottom[3].second}},   // Top-right
        ChunkVertex{{x + 0.5f, y + -0.5f, z + -0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), {tex_bottom[1].first, tex_bottom[1].second}},  // Bottom-right
    };
}

std::vector<ChunkVertex> GetFace(BLOCKFACE face, BlockType type, float x, float y, float z)
{
    switch (face)
    {
    case BLOCKFACE::FRONT:
        return GetFrontFace(x, y, z, type);
    case BLOCKFACE::BACK:
        return GetBackFace(x, y, z, type);
    case BLOCKFACE::LEFT:
        return GetLeftFace(x, y, z, type);
    case BLOCKFACE::RIGHT:
        return GetRightFace(x, y, z, type);
    case BLOCKFACE::TOP:
        return GetTopFace(x, y, z, type);
    case BLOCKFACE::BOTTOM:
        return GetBottomFace(x, y, z, type);
    default:
        return {};
    }
}

BlockUpdate::BlockUpdate()
{
}

BlockUpdate::BlockUpdate(int x, int y, int z, BlockType block) : x(x), y(y), z(z), type(block)
{

}

BlockUpdate::BlockUpdate(const BlockUpdate &other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    type = other.type;
}
