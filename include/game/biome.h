#ifndef BIOME_H
#define BIOME_H
#include "block.h"

enum BIOMETYPE {
    HILLS,
    PLAINS,
};

namespace BIOME
{
    int GetSurfaceVariation(BIOMETYPE biome);
    BlockType Hills_GetBlockType(int y, int surface);
};








#endif

