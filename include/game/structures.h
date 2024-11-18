#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef enum STRUCTURETYPE {
    NO_STRUCTURE,
    tree1,
}STRUCTURETYPE;


#include "block.h"
#include "chunk.h"
#include <math.h>


#define STRUCTURES_GEN_DISTANCE (std::max(CHUNK_DISTANCE - 4.0f, 1.0f))
#define STRUCTURES_GEN_THRESHOLD 0.999

struct StructureComponent {
    int x;
    int y;
    int z;
    BlockType block;    
};




namespace structure {
   std::vector<BlockUpdate> GenVertices(STRUCTURETYPE type, int x, int y, int z);

};





static const std::vector<StructureComponent> Schematic_tree1 = {
    {0, 1, 0,   BlockType::Dirt}, // base of tree
    {0, 2, 0,   BlockType::Dirt},
    {0, 3, 0,   BlockType::Dirt},
    {0, 4, 0,   BlockType::Dirt},
    {1, 4, 0,   BlockType::Dirt},
    {0, 4, 1,   BlockType::Dirt},
    {-1, 4, 0,  BlockType::Dirt},
    {0, 4, -1,  BlockType::Dirt},
    {1, 4, 1,   BlockType::Dirt},
    {-1, 4, -1, BlockType::Dirt},
    {1, 4, -1,   BlockType::Dirt},
    {-1, 4, 1, BlockType::Dirt},
    {0, 5, 0,   BlockType::Dirt},
    {1, 5, 0,   BlockType::Dirt},
    {0, 5, 1,   BlockType::Dirt},
    {-1, 5, 0,  BlockType::Dirt},
    {0, 5, -1,  BlockType::Dirt},
    {1, 5, 1,   BlockType::Dirt},
    {-1, 5, -1, BlockType::Dirt},
    {1, 5, -1,   BlockType::Dirt},
    {-1, 5, 1, BlockType::Dirt},
    {0, 6, 0,   BlockType::Dirt},
};



static const std::vector<StructureComponent> Schematic_tree2 = {
    {0, 1, 0,   BlockType::OakWood}, // base of tree
    {0, 2, 0,   BlockType::OakWood},
    {0, 3, 0,   BlockType::OakWood},
    {0, 4, 0,   BlockType::OakWood},
    {1, 4, 0,   BlockType::OakLeaf},
    {0, 4, 1,   BlockType::OakLeaf},
    {-1, 4, 0,  BlockType::OakLeaf},
    {0, 4, -1,  BlockType::OakLeaf},
    {1, 4, 1,   BlockType::OakLeaf},
    {-1, 4, -1, BlockType::OakLeaf},
    {0, 5, 0,   BlockType::OakLeaf},
    {1, 5, 0,   BlockType::OakLeaf},
    {0, 5, 1,   BlockType::OakLeaf},
    {-1, 5, 0,  BlockType::OakLeaf},
    {0, 5, -1,  BlockType::OakLeaf},
    {1, 5, 1,   BlockType::OakLeaf},
    {-1, 5, -1, BlockType::OakLeaf},
    {0, 6, 0,   BlockType::OakLeaf},
};




#endif