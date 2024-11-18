#include "biome.h"
#include "chunk.h"


int BIOME::GetSurfaceVariation(BIOMETYPE biome)
{
    switch(biome)
    {
        case BIOMETYPE::HILLS: return 16;
        case BIOMETYPE::PLAINS: return 3;
        case BIOMETYPE::Desert: return 3;
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

BlockType BIOME::Plains_GetBlockType(int x, int y, int z, int surface)
{
    if (y == surface)
        return BlockType::DirtSurface;
    if (surface - y < 6)
        return BlockType::Dirt;
    else
        return BlockType::Stone;
}

BlockType BIOME::ExtremeHills_GetBlockType(int x, int y, int z, int surface)
{
    if (y > 85)
        return BlockType::Stone;
    else
    {
        if (y == surface)
            return BlockType::DirtSurface;
        if (surface - y < 6)
            return BlockType::Dirt;
        else
            return BlockType::Stone;
    }
}

BlockType BIOME::Desert_GetBlockType(int x, int y, int z, int surface)
{
    if (y == surface)
        return BlockType::Sand;
    if (surface - y < 6)
        return BlockType::Sand; // sandstone
    else
        return BlockType::Stone;
}

STRUCTURETYPE BIOME::GetStructure(BIOMETYPE biome, float structureNoise)
{
    switch (biome)
    {
        case BIOMETYPE::HILLS: return Hills_GetStructureType(structureNoise);
        case BIOMETYPE::PLAINS: return Plains_GetStructureType(structureNoise);
        case BIOMETYPE::Desert: return Desert_GetStructureType(structureNoise);
        default: return STRUCTURETYPE::NO_STRUCTURE;
    }
}

STRUCTURETYPE BIOME::Hills_GetStructureType(float structureNoise)
{
    if (structureNoise > STRUCTURES_GEN_THRESHOLD)
        return STRUCTURETYPE::tree1;
    return STRUCTURETYPE::NO_STRUCTURE;
}

STRUCTURETYPE BIOME::Plains_GetStructureType(float structureNoise)
{
    if (structureNoise > STRUCTURES_GEN_THRESHOLD)
        return STRUCTURETYPE::tree1;

    return STRUCTURETYPE::NO_STRUCTURE;
}

STRUCTURETYPE BIOME::Desert_GetStructureType(float structureNoise)
{
    if (structureNoise > STRUCTURES_GEN_THRESHOLD)
        return STRUCTURETYPE::tree1;

    return STRUCTURETYPE::NO_STRUCTURE;
}
