#include "renderer.h"
Renderer renderer;
extern Logger logger;
extern GLManager gl;
Renderer::Renderer()
{
    m_RenderObjects.reserve(100);
}

Renderer::~Renderer()
{
}


void Renderer::AddRenderObject(RenderObject *obj)
{
    // acquire RenderLock whenever modifying the render objects list

    std::unique_lock lock(m_RenderLock);
    switch(obj->GetType())
    {
        case OBJECTYPE::Regular: m_RenderObjects.push_back(obj); break;
        case OBJECTYPE::LightSource: m_LightingObjects = obj; break;
        default:
            logger.Log(LOGTYPE::WARNING, "Renderer::AddRenderObject() --> attempted to add RenderObject with invalid OBJECTTYPE.\n");
    }
    

}

void Renderer::RenderAllObjects()
{
    std::unique_lock lock(m_RenderLock);

    HandleLightSources();
    

    int i = m_RenderObjects.size();
    for (int x = 0; x < i;  x++)
    {
        auto& obj = m_RenderObjects[x];
        obj->Render();
    }
    lock.unlock();
}

/*
    Traverses the RenderObject list and uses a simple distance formula to get the closest one

    If there are no objects returns nullptr
*/
RenderObject *Renderer::FindClosestObject(const glm::vec3 &pos)
{
    RenderObject* ret_obj = nullptr;
    float closest = 10e10;
    std::unique_lock lock(m_RenderLock);
    for (auto& obj: m_RenderObjects)
    {
        float dist = glm::distance(pos, obj->GetPosition());
        if (dist < closest && dist < MOUSE_CLICK_OBJ_SEL_THRESHOLD)
        {
            ret_obj = obj;
            closest = dist;
        }
    }

    float dist = glm::distance(pos, m_LightingObjects->GetPosition());
    if (dist < closest && dist < MOUSE_CLICK_OBJ_SEL_THRESHOLD)
    {
        ret_obj = m_LightingObjects;
        closest = dist;
    }

    return ret_obj;
}



/*
    This function will render the light sources with the appropriate shaders and then
    set the appropriate uniforms for calculations by the other objects
*/
void Renderer::HandleLightSources()
{

    if (!m_LightingObjects)
        printf("not found!\n");
    m_LightingObjects->Render();
    




    CalculateAndSetLightingUniforms();
}

void Renderer::CalculateAndSetLightingUniforms()
{
}
