#include "biome.h"
#include "chunk.h"


int BIOME::GetSurfaceVariation(BIOMETYPE biome)
{
    switch(biome)
    {
        case BIOMETYPE::HILLS: return 16;
        case BIOMETYPE::PLAINS: return 3;
        default: return 0;
    }
}

BlockType BIOME::Hills_GetBlockType(int x, int y, int z, int surface)
{
    if (y == surface)
        return BlockType::DirtSurface;
    if (surface - y < 6)
        return BlockType::Dirt;
    else
        return BlockType::Stone;
    
}
