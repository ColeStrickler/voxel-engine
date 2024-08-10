#ifndef BIOME_H
#define BIOME_H
#include "block.h"
#include "structures.h"
enum BIOMETYPE {
    HILLS,
    PLAINS,
    EXTREME_HILLS,
    Desert,
};

namespace BIOME
{
    int GetSurfaceVariation(BIOMETYPE biome);
    BlockType Hills_GetBlockType(int x, int y, int z, int surface);
    BlockType Plains_GetBlockType(int x, int y, int z, int surface);
    BlockType ExtremeHills_GetBlockType(int x, int y, int z, int surface);
    BlockType Desert_GetBlockType(int x, int y, int z, int surface);

    STRUCTURETYPE GetStructure(BIOMETYPE biome, float structureNoise);
    STRUCTURETYPE Hills_GetStructureType(float structureNoise);
    STRUCTURETYPE Plains_GetStructureType(float structureNoise);
    STRUCTURETYPE Desert_GetStructureType(float structureNoise);

};






#endif

