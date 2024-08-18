#include "gpu_allocator.h"

extern Logger logger;
int deleted = 0;
int created = 0;
GPUBuddyNode::GPUBuddyNode(bool isFree, uint64_t s_size, uint64_t offset, GPUBuddyNode* parent, int childNum, int totalChildren): free(isFree), size(s_size), offset(offset), parent(parent),\
    leftNode(leftNode), child_no(childNum), total_children(totalChildren)
{
    left = nullptr;
    right = nullptr;

    children = new GPUBuddyNode*[totalChildren];
    for (int i = 0; i < total_children; i++)
        children[i] = 0;
}

GPUBuddyNode::~GPUBuddyNode()
{
    delete children;
}

/*
    Checks if this node is itself housing the allocation
*/
bool GPUBuddyNode::isOccupied()
{
    bool occupied = true;
    for (int i = 0; i < total_children; i++)
    {
        if (children[i] != nullptr)
        {
            occupied = false;
            break;
        }
        
    }
    return occupied && !free;
}

std::vector<int> GPUBuddyNode::GetChildrenCanUnset()
{
     std::vector<int> toUnset;

    for (int i = 0; i < total_children; i++)
    {
        if (children[i] == nullptr)
            toUnset.push_back(i);
    }
    return toUnset;
}


GPUAllocator::GPUAllocator(float percentMemory, uint32_t alignment, int treeFanout) : m_VertexAlignment(alignment), m_Wasted(0)
{
    assert(percentMemory > 0.0f && percentMemory <= 1.0f);


    nvmlReturn_t result = nvmlInit();
    if (result != NVML_SUCCESS) {
        logger.Log(LOGTYPE::ERROR, "GPUAllocator::GPUAllocator() Failed to initialize NVML: " + std::string(nvmlErrorString(result)));
        return;
    }

    nvmlDevice_t device;
    result = nvmlDeviceGetHandleByIndex(0, &device);
    if (result != NVML_SUCCESS) {
        logger.Log(LOGTYPE::ERROR, "GPUAllocator::GPUAllocator() Failed to get device handle: " + std::string(nvmlErrorString(result)));
        nvmlShutdown();
        return;
    }


    char name[NVML_DEVICE_NAME_BUFFER_SIZE];
    result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
    if (result != NVML_SUCCESS) 
        logger.Log(LOGTYPE::ERROR, "GPUAllocator::GPUAllocator() Failed to get device name: " + std::string(nvmlErrorString(result)));
    else
        m_DeviceName = std::string(name);




    nvmlMemory_t memInfo;
    if((result = nvmlDeviceGetMemoryInfo(device, &memInfo)) != NVML_SUCCESS) {
        logger.Log(LOGTYPE::ERROR, "GPUAllocator::GPUAllocator()  Failed to fetch device memory info: " + std::string(nvmlErrorString(result)));
        nvmlShutdown();
        return;
    } 

    // ensure we have a power of 2
    m_MaxMemory = percentMemory * memInfo.free;
    m_AllocatorCapacity = POWER_OF_2(static_cast<uint64_t>(percentMemory * memInfo.free));
    m_UsedMemory = 0;

    m_RootNode = new GPUBuddyNode(true, m_AllocatorCapacity, 0, nullptr, 0, treeFanout);
    m_VB = new VertexBuffer(m_AllocatorCapacity);
    //m_VB->UnsetData(0, m_AllocatorCapacity);
    nvmlShutdown();
}



GPUAllocator::~GPUAllocator()
{
}

VertexBuffer *GPUAllocator::GetVertexBuffer()
{
    return m_VB;
}

void GPUAllocator::FreeData(const std::string &key)
{
    EMIT_PROFILE_TOKEN
    auto& nodes = m_AllocTracker[key];
    for (auto& node : nodes)
    {    FreeNode(node);
        //printf("free data %lld\n", node.offset);
        //m_VB->UnsetData(node.offset, node.size);
    }
    //glInvalidateBufferSubData --> this will work
    m_AllocTracker.erase(key);
    
}

bool GPUAllocator::PutData(const std::string &key, void *data, uint64_t sizeInBytes, bool realloc)
{
   // printf("size: %lld\n", sizeInBytes);
    while (m_UsedMemory + sizeInBytes > m_AllocatorCapacity)
    {
        if (m_AllocatorCapacity == m_MaxMemory)
        {
            printf("reached capacity\n");
            return false;
        }
        
        m_AllocatorCapacity = std::min(m_AllocatorCapacity*2, m_MaxMemory);
        
        m_VB->Grow(m_AllocatorCapacity, m_UsedMemory);
    }
    m_VB->SetData(data, m_UsedMemory, sizeInBytes);
    //m_AllocTracker[key].push_back({m_UsedMemory, sizeInBytes});
    
    m_UsedMemory += sizeInBytes;
    return true;
}

bool GPUAllocator::PutData(const std::string &key, void *data, uint64_t sizeInBytes)
{
    EMIT_PROFILE_TOKEN
    GPUBuddyNode* allocatedNode = FindAndCreateNode(m_RootNode, sizeInBytes+m_VertexAlignment);
    if (allocatedNode == nullptr)
    {
        //printf("reached capacity\n");
        return false;
    }
    m_AllocTracker[key].push_back(allocatedNode); // allocation was successful
    /*
        Now that we have confirmed we have space we can actually put it on the GPU
    */

    uint64_t aligned_addr = (allocatedNode->offset + (m_VertexAlignment)) - ((allocatedNode->offset + m_VertexAlignment)%m_VertexAlignment);
    uint64_t address = allocatedNode->offset % m_VertexAlignment == 0 ? allocatedNode->offset : aligned_addr;
   
    m_VB->SetData(data, address, sizeInBytes);
    return true;
}

GPUBuddyNode *GPUAllocator::FindAndCreateNode(GPUBuddyNode *currNode, uint64_t bytesRequested)
{
    assert(currNode != nullptr);
    assert(bytesRequested > 0);
    //if (bytesRequested < MINALLOC_SIZE)
    //    return nullptr;
    GPUBuddyNode* node = nullptr;
    //printf("currNode->size = %lldd Total nodes: %d\n", currNode->size, nodeCount);
    if (bytesRequested > currNode->size || currNode->offset >= m_AllocatorCapacity) // traversed too far
    {
        return nullptr;
    }
    else if (currNode->isOccupied()) // node is already housing an allocation
    {
        return nullptr;
    }
    else
    {
        if (((bytesRequested > currNode->size/currNode->total_children && bytesRequested <= currNode->size) || (bytesRequested <= MINALLOC_SIZE && currNode->size <= MINALLOC_SIZE)) && currNode->free) // this is the correct size to fill the allocation
        {
            m_Wasted += currNode->size - bytesRequested;
            currNode->free = false;
            return currNode;
        }


        for (int i = 0; i < currNode->total_children; i++)
        {
            if (currNode->children[i] == nullptr)
            {
                nodeCount++;
                currNode->children[i] = new GPUBuddyNode(true, currNode->size/currNode->total_children,\
                 currNode->offset + floor(i * currNode->size/currNode->total_children), currNode, i, currNode->total_children);
            }
            node = FindAndCreateNode(currNode->children[i], bytesRequested);
            if (node != nullptr)
            {
                currNode->free = false;
                return node;
            }
            else
            {
                if (currNode->children[i]->free)
                {
                    nodeCount--;
                    delete currNode->children[i];
                    currNode->children[i] = nullptr;
                }
            }

        }
    }

    return nullptr;
}

void GPUAllocator::FreeNode(GPUBuddyNode *currNode)
{
    assert(currNode != nullptr);

    if (currNode->parent == nullptr) // reached root node, nothing left to do
    {
        m_VB->UnsetData(currNode->offset, currNode->size);
        return;
    }

    bool goodLeft = currNode->left == nullptr;
    bool goodRight = currNode->right == nullptr;

    auto toUnset = currNode->GetChildrenCanUnset();
    bool canDeleteNode = toUnset.size() == currNode->total_children;

    if (canDeleteNode)
    {
        auto parent = currNode->parent;
        parent->children[currNode->child_no] = nullptr;
        nodeCount--;
        delete currNode;
        FreeNode(parent);
    }
    else
    {
        for (auto& child_index: toUnset)
        {
            m_VB->UnsetData(currNode->offset + floor(child_index *currNode->size/currNode->total_children), currNode->size/currNode->total_children);
        }
    }
}

