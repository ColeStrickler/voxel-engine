#include "chunk.h"

Chunk::Chunk(int x, int z, ShaderProgram* sp) : m_xCoord(x), m_zCoord(z)
{
    GenerateChunk();
    GenerateChunkMesh(sp);
}   

Chunk::~Chunk()
{
}

void Chunk::GenerateChunkMesh(ShaderProgram* sp)
{
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
            for (int y = 0; y < MAX_CHUNK_HEIGHT; y++)
            {
                auto& block = m_Blocks[x][y][z];
                if (block.isActive())
                    BlockGenVertices(block, x, y, z);
            }
        }
    }

    BufferLayout* vertex_layout = new BufferLayout({new BufferElement("COORDS", ShaderDataType::Float3, false),
          new BufferElement("faceBlockType", ShaderDataType::Int, false),  new BufferElement("reserved", ShaderDataType::Int, false)});

    m_IB = new IndexBuffer(m_Indices.data(), m_Indices.size());
    m_VB = new VertexBuffer((float*)m_Vertices.data(), m_Vertices.size()*sizeof(ChunkVertex));
    m_VB->SetLayout(vertex_layout);
    m_VA = new VertexArray();
    m_VA->AddVertexBuffer(m_VB);
    m_VA->AddIndexBuffer(m_IB);
    m_RenderObj = new RenderObject(m_VA, m_VB, sp, m_IB, OBJECTYPE::ChunkMesh);
    m_RenderObj->Translate(glm::vec3(m_xCoord*CHUNK_WIDTH, 0.0f, m_zCoord*CHUNK_WIDTH));
}

void Chunk::BlockGenVertices(Block &block, int x, int y, int z)
{
    printf("here %d, %d, %d\n", x, y, z);
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
        cubeIndices[i] += m_Indices.size();
    m_Indices.insert(m_Indices.end(), cubeIndices.begin(), cubeIndices.end());

    std::vector<ChunkVertex> vertvec = {
        // Front Face
        ChunkVertex {{x+-0.5f, y+-0.5f,  z+0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), 0}, // Bottom-left
        ChunkVertex {{x+ 0.5f, y+-0.5f,  z+0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), 0}, // Bottom-right
        ChunkVertex {{x+-0.5f, y+ 0.5f,  z+0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), 0}, // Top-left
        ChunkVertex {{x+ 0.5f, y+ 0.5f,  z+0.5f}, PACK_FACEBLOCK(BLOCKFACE::FRONT, type), 0}, // Top-right
        // Back Face
        ChunkVertex {{x+-0.5f, y+-0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), 0}, // Bottom-left
        ChunkVertex {{x+ 0.5f, y+-0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), 0}, // Bottom-right
        ChunkVertex {{x+-0.5f, y+ 0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), 0}, // Top-left
        ChunkVertex {{x+ 0.5f, y+ 0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::BACK, type), 0}, // Top-right
        // Left Face
        ChunkVertex{{x+-0.5f, y+-0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), 0}, // Bottom-left
        ChunkVertex{{x+-0.5f, y+-0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), 0}, // Bottom-right
        ChunkVertex{{x+-0.5f, y+ 0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), 0}, // Top-left
        ChunkVertex{{x+-0.5f, y+ 0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::LEFT, type), 0}, // Top-right
        // Right Face
        ChunkVertex{{x+0.5f, y+-0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), 0}, // Bottom-left
        ChunkVertex{{x+0.5f, y+-0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), 0}, // Bottom-right
        ChunkVertex{{x+0.5f, y+ 0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), 0}, // Top-left
        ChunkVertex{{x+0.5f, y+ 0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::RIGHT, type), 0}, // Top-right
        // Top Face
        ChunkVertex {{x+-0.5f,  y+0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), 0}, // Bottom-left
        ChunkVertex {{x+ 0.5f,  y+0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), 0}, // Bottom-right
        ChunkVertex {{x+-0.5f,  y+0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), 0}, // Top-left
        ChunkVertex {{x+ 0.5f,  y+0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::TOP, type), 0}, // Top-right
        // Bottom Face
        ChunkVertex{{x+-0.5f, y+-0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), 0}, // Bottom-left
        ChunkVertex{{x+ 0.5f, y+-0.5f, z+-0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), 0}, // Bottom-right
        ChunkVertex{{x+-0.5f, y+-0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), 0}, // Top-left
        ChunkVertex{{x+ 0.5f, y+-0.5f, z+ 0.5f}, PACK_FACEBLOCK(BLOCKFACE::BOTTOM, type), 0}, // Top-right
    };
    m_Vertices.insert(m_Vertices.end(), vertvec.begin(), vertvec.end());
}

void Chunk::GenerateChunk()
{
    
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
            for (int y = 0; y < MAX_CHUNK_HEIGHT; y++)
            {
                auto& block = m_Blocks[x][y][z];
                block.setType(BlockType::Dirt);
               // printf("here %d,%d,%d\n", x, y, z);
                if (x == 0 || z == 0 || x == 15 || z == 15)
                    block.setActive(true);
            }
        }
    }
}


