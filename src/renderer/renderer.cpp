#include "renderer.h"
Renderer renderer;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}


void Renderer::AddRenderObject(RenderObject *obj)
{
    m_RenderObjects.push_back(obj);
}

void Renderer::RenderAllObjects()
{
    // Use this to keep track of when we're done
    for (auto& obj: m_RenderObjects)
    {
        obj->Render();
    }

}

/*
    Traverses the RenderObject list and uses a simple distance formula to get the closest one

    If there are no objects returns nullptr
*/
RenderObject *Renderer::FindClosestObject(const glm::vec3 &pos)
{
    RenderObject* ret_obj = nullptr;
    float closest = 10e10;
    
    for (auto& obj: m_RenderObjects)
    {
        float dist = glm::distance(pos, obj->GetPosition());
        if (dist < closest)
        {
            ret_obj = obj;
            closest = dist;
        }
    }
    return ret_obj;
}
