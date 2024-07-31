#include "block.h"

extern Logger logger;


Block::Block()
{
}

Block::Block(BlockType type) : m_Type(type)
{
    setActive(false);
}

Block::~Block()
{
}

bool Block::setActive(bool bActive)
{
    m_bIsActive = bActive;
}

std::vector<BlockVertex> Block::GenBlockVertices(BlockType blocktype)
{
    assert(blocktype >= 0 && blocktype < BlockType::BLOCKMAX); 
    std::vector<std::pair<uint32_t, uint32_t>> face_indices = BlockFaceIndexes[blocktype];
    std::vector<BlockVertex> newVertices = cubeVertices; // copy new vertex data

    // replace the texture coords with the proper index into the texture atlas
    auto& front_index = face_indices[0];
    auto& back_index = face_indices[1];
    auto& left_index = face_indices[2];
    auto& right_index = face_indices[3];
    auto& top_index = face_indices[4];
    auto& bottom_index = face_indices[5];

    // get the normalized texture coordinates for each face
    auto front_coords   = GenTextureCoordBlockFace(front_index);
    auto back_coords    = GenTextureCoordBlockFace(back_index);
    auto left_coords    = GenTextureCoordBlockFace(left_index);
    auto right_coords   = GenTextureCoordBlockFace(right_index);
    auto top_coords     = GenTextureCoordBlockFace(top_index);
    auto bottom_coords  = GenTextureCoordBlockFace(bottom_index);
    
    // set front texture coords
    newVertices[0].texCoords[0] = front_coords[2].first;    // bottom left
    newVertices[0].texCoords[1] = front_coords[2].second;   // bottom left
    newVertices[1].texCoords[0] = front_coords[3].first;    // bottom right
    newVertices[1].texCoords[1] = front_coords[3].second;   // bottom right
    newVertices[2].texCoords[0] = front_coords[0].first;    // top left
    newVertices[2].texCoords[1] = front_coords[0].second;   // top left
    newVertices[3].texCoords[0] = front_coords[1].first;    // top right
    newVertices[3].texCoords[1] = front_coords[1].second;   // top right

    // set back texture coords
    newVertices[4].texCoords[0] = back_coords[2].first;    // bottom left
    newVertices[4].texCoords[1] = back_coords[2].second;   // bottom left
    newVertices[5].texCoords[0] = back_coords[3].first;    // bottom right
    newVertices[5].texCoords[1] = back_coords[3].second;   // bottom right
    newVertices[6].texCoords[0] = back_coords[0].first;    // top left
    newVertices[6].texCoords[1] = back_coords[0].second;   // top left
    newVertices[7].texCoords[0] = back_coords[1].first;    // top right
    newVertices[7].texCoords[1] = back_coords[1].second;   // top right

    // set left texture coords
    newVertices[8].texCoords[0] = left_coords[2].first;    // bottom left
    newVertices[8].texCoords[1] = left_coords[2].second;   // bottom left
    newVertices[9].texCoords[0] = left_coords[3].first;    // bottom right
    newVertices[9].texCoords[1] = left_coords[3].second;   // bottom right
    newVertices[10].texCoords[0] = left_coords[0].first;    // top left
    newVertices[10].texCoords[1] = left_coords[0].second;   // top left
    newVertices[11].texCoords[0] = left_coords[1].first;    // top right
    newVertices[11].texCoords[1] = left_coords[1].second;   // top right

    // set right texture coords
    newVertices[12].texCoords[1] = right_coords[2].second;   // bottom left
    newVertices[12].texCoords[0] = right_coords[2].first;    // bottom left
    newVertices[13].texCoords[0] = right_coords[3].first;    // bottom right
    newVertices[13].texCoords[1] = right_coords[3].second;   // bottom right
    newVertices[14].texCoords[0] = right_coords[0].first;    // top left
    newVertices[14].texCoords[1] = right_coords[0].second;   // top left
    newVertices[15].texCoords[0] = right_coords[1].first;    // top right
    newVertices[15].texCoords[1] = right_coords[1].second;   // top right

    // set top texture coords
    newVertices[16].texCoords[1] = top_coords[2].second;   // bottom left
    newVertices[16].texCoords[0] = top_coords[2].first;    // bottom left
    newVertices[17].texCoords[0] = top_coords[3].first;    // bottom right
    newVertices[17].texCoords[1] = top_coords[3].second;   // bottom right
    newVertices[18].texCoords[0] = top_coords[0].first;    // top left
    newVertices[18].texCoords[1] = top_coords[0].second;   // top left
    newVertices[19].texCoords[0] = top_coords[1].first;    // top right
    newVertices[19].texCoords[1] = top_coords[1].second;   // top right

    // set bottom texture coords
    newVertices[20].texCoords[1] = bottom_coords[2].second;   // bottom left
    newVertices[20].texCoords[0] = bottom_coords[2].first;    // bottom left
    newVertices[21].texCoords[0] = bottom_coords[3].first;    // bottom right
    newVertices[21].texCoords[1] = bottom_coords[3].second;   // bottom right
    newVertices[22].texCoords[0] = bottom_coords[0].first;    // top left
    newVertices[22].texCoords[1] = bottom_coords[0].second;   // top left
    newVertices[23].texCoords[0] = bottom_coords[1].first;    // top right
    newVertices[23].texCoords[1] = bottom_coords[1].second;   // top right


    return newVertices;
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
    printf("{{%.2f, %.2f, %.2f}, {%.2f, %.2f}, {%.2f, %.2f, %.2f}}\n", v.position[0],v.position[1],v.position[2],v.texCoords[0],v.texCoords[1],v.normal[0],v.normal[1],v.normal[2]);
}
