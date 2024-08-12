#ifndef GPU_ALLOC_H
#define GPU_ALLOC_H
#include <nvml.h>
#include <map>
#include <unordered_set>
#include "logger.h"
#include "glbuffer.h"
#define MINALLOC_SIZE 4096

#define POWER_OF_2(x)((x) - (x%2))

struct GPUBuddyNode
{
    GPUBuddyNode(bool isFree, uint64_t s_size, uint64_t offset, GPUBuddyNode* parent, bool leftNode);
    bool free;
    bool leftNode; // left node of parent
    uint64_t size;
    uint64_t offset;
    GPUBuddyNode* parent;
    GPUBuddyNode* left;
    GPUBuddyNode* right;
};


class GPUAllocator 
{
public:
    GPUAllocator(float percentMemory);
    ~GPUAllocator();
    

    void FreeData(const std::string& key);
    bool PutData(const std::string& key, void* data, uint64_t sizeInBytes);
    GPUBuddyNode* FindAndCreateNode(GPUBuddyNode *currNode, uint64_t bytesRequested);
    int nodeCount;
private:
    void FreeNode(GPUBuddyNode* currNode);

    

    std::string m_DeviceName;
    uint64_t m_AllocatorCapacity;

    GPUBuddyNode* m_RootNode;
    VertexBuffer* m_VB;
    std::unordered_map<std::string, std::vector<GPUBuddyNode*>> m_AllocTracker;
};


#endif