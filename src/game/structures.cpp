#include "structures.h"

std::vector<BlockUpdate> structure::GenVertices(STRUCTURETYPE type, int x, int y, int z)
{
    std::vector<BlockUpdate> ret;
    switch (type)
    {
        case STRUCTURETYPE::tree1:
        {
            for (auto &sc : Schematic_tree1)
            {
                auto nx = x + sc.x;
                auto ny = y + sc.y;
                auto nz = z + sc.z;
                auto blockType = sc.block;
                BlockUpdate update(nx,ny,nz, blockType);
                ret.push_back(update);
            }
        }
        default:
            break;
    }
    return ret;
}