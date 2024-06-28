#ifndef CHUNK_H
#define CHUNK_H
#include <vector>
#include "world.h"
#include "block.h"
#define MAX_CHUNK_HEIGHT 128
#define MIN_CHUNK_HEIGHT 0
#define CHUNK_WIDTH 16


class Chunk
{
public:
    Chunk();
    ~Chunk();

private:
    void GenerateChunk();
    void Render();
    std::vector<Block> m_Blocks;
};

#endif 