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
    GPUBuddyNode(bool isFree, uint64_t s_size, uint64_t offset, GPUBuddyNode* parent, int childNum, int totalChildren);
    ~GPUBuddyNode();
    bool isOccupied();
    std::vector<int> GetChildrenCanUnset();
    bool free;
    bool leftNode; // left node of parent
    uint64_t size;
    uint64_t offset;
    int child_no;
    int total_children;
    GPUBuddyNode* parent;
    GPUBuddyNode** children;
    GPUBuddyNode* left;
    GPUBuddyNode* right;

    GPUBuddyNode* next;
    GPUBuddyNode* prev;
};


struct GPUAllococation
{
    uint64_t offset;
    uint64_t size;
};


class GPUAllocator 
{
public:
    GPUAllocator(float percentMemory, uint32_t alignment, int treeFanout=2);
    ~GPUAllocator();
    
    void InsertHead(GPUBuddyNode* node);
    void InsertTail(GPUBuddyNode* node);


    /*
        Lets try and make a linked list and copy the buffers around on the GPU instead so they are contiguous
    */
    VertexBuffer* GetVertexBuffer();

    void FreeData(const std::string& key);

    bool PutData(const std::string& key, void* data, uint64_t sizeInBytes, bool realloc);
    bool PutData(const std::string& key, void* data, uint64_t sizeInBytes);
    GPUBuddyNode* FindAndCreateNode(GPUBuddyNode *currNode, uint64_t bytesRequested);
    int nodeCount;

    uint32_t m_VertexAlignment;
    uint64_t m_MaxMemory;
    uint64_t m_UsedMemory;
    uint64_t m_AllocatorCapacity;

    uint64_t m_Wasted;
private:
    void FreeNode(GPUBuddyNode* currNode);

    

    std::string m_DeviceName;
    GPUBuddyNode* m_RootNode;

    //GPUBuddyNode head;


    VertexBuffer* m_VB;
    std::unordered_map<std::string, std::vector<GPUBuddyNode*>> m_AllocTracker;
};


#endif