#include "block.h"

extern Logger logger;


Block::Block() : m_Active_Type(0)
{
   
}

Block::~Block()
{
}

bool Block::isActive() const  {return (bool)((m_Active_Type >> 15) & 1);};

void Block::setActive(bool bActive)
{
    m_Active_Type &= 0x7fff; // clear bit 15
    m_Active_Type |= ((bActive & 1) << 15); // set bit 15 to bActive
}

void Block::setType(BlockType type)
{
    m_Active_Type &= (1 << 15); // clear bottom bits
    m_Active_Type |= ((unsigned short)type & 0x7fff);
}

BlockType Block::getBlockType() const
{
    return (BlockType)(m_Active_Type & 0x7fff);
}

std::vector<std::pair<float, float>> Block::GenBlockVertices(BlockType blocktype, BLOCKFACE face)
{
    assert((unsigned short)blocktype >= 0 && (unsigned short)blocktype < (unsigned short)BlockType::BLOCKMAX); 
    std::vector<std::pair<uint32_t, uint32_t>> face_indices = BlockFaceIndexes[(unsigned short)blocktype];
    std::vector<BlockVertex> newVertices = cubeVertices; // copy new vertex data

    // replace the texture coords with the proper index into the texture atlas
    auto& front_index = face_indices[0];
    auto& back_index = face_indices[1];
    auto& left_index = face_indices[2];
    auto& right_index = face_indices[3];
    auto& top_index = face_indices[4];
    auto& bottom_index = face_indices[5];

    switch (face)
    {
        // {bottom left, bottom right, top left, top right}
        case BLOCKFACE::FRONT:
        {
            auto coords   = GenTextureCoordBlockFace(front_index);
            return {{coords[2].first, coords[2].second}, {coords[3].first, coords[3].second}, {coords[0].first, coords[0].second}, \
            {coords[1].first, coords[1].second}};
        }
        case BLOCKFACE::BACK:
        {
            auto coords   = GenTextureCoordBlockFace(back_index);
            return {{coords[2].first, coords[2].second}, {coords[3].first, coords[3].second}, {coords[0].first, coords[0].second}, \
            {coords[1].first, coords[1].second}};
        }
        case BLOCKFACE::LEFT:
        {
            auto coords   = GenTextureCoordBlockFace(left_index);
            return {{coords[2].first, coords[2].second}, {coords[3].first, coords[3].second}, {coords[0].first, coords[0].second}, \
            {coords[1].first, coords[1].second}};
        }
        case BLOCKFACE::RIGHT:
        {
            auto coords   = GenTextureCoordBlockFace(right_index);
            return {{coords[2].first, coords[2].second}, {coords[3].first, coords[3].second}, {coords[0].first, coords[0].second}, \
            {coords[1].first, coords[1].second}};
        }
        case BLOCKFACE::TOP:
        {
            auto coords   = GenTextureCoordBlockFace(top_index);
            return {{coords[2].first, coords[2].second}, {coords[3].first, coords[3].second}, {coords[0].first, coords[0].second}, \
            {coords[1].first, coords[1].second}};
        }
        case BLOCKFACE::BOTTOM:
        {
            auto coords   = GenTextureCoordBlockFace(bottom_index);
            return {{coords[2].first, coords[2].second}, {coords[3].first, coords[3].second}, {coords[0].first, coords[0].second}, \
            {coords[1].first, coords[1].second}};
        }
        default:
            logger.Log(LOGTYPE::ERROR, "Block::GenBlockVertices() --> bad blockface");
            assert(0 > 1);

    }
}


// returns top left, top right, bottom left, bottom right
std::vector<std::pair<float,float>> Block::GenTextureCoordBlockFace(std::pair<uint32_t, uint32_t> TextureAtlasIndex)
{
    float row = static_cast<float>(NUMROW - (TextureAtlasIndex.first));
    float col = static_cast<float>(TextureAtlasIndex.second);

    float xmin =   (col*TEXTURE_WIDTH)/TEXTURE_ATLAS_WIDTH;
    float ymin =   ((row-1.0f)*TEXTURE_WIDTH)/TEXTURE_ATLAS_HEIGHT;
    float xmax =   ((col+1.0f)*TEXTURE_WIDTH)/TEXTURE_ATLAS_WIDTH;
    float ymax =   ((row)*TEXTURE_WIDTH)/TEXTURE_ATLAS_HEIGHT;
    //printf("%.3f,%.3f,%.3f,%.3f\n", xmin, ymin, xmax, ymax);
    // top left, top right, bottom left, bottom right, 0.0f is bottom
    std::vector<std::pair<float, float>> ret = {{xmin,ymax},{xmax,ymax},{xmin,ymin},{xmax,ymin}};
    return ret;
}

void PrintBlockVertex(BlockVertex &v)
{

    //while(1);
    printf("{{%.2f, %.2f, %.2f}, {%.2f, %.2f}, {%.2f, %.2f, %.2f}}\n", v.position[0],v.position[1],v.position[2],v.texCoords[0],v.texCoords[1],v.normal[0],v.normal[1],v.normal[2]);
}
