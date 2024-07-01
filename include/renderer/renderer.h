#ifndef RENDERER_H
#define RENDERER_H
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include "gl.h"
#include "renderobject.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();
    void AddRenderObject(RenderObject* obj);
    void RenderAllObjects();
    RenderObject* FindClosestObject(const glm::vec3& pos);

    RenderObject* GetLighting() const {return m_LightingObjects;}
private:
    std::vector<RenderObject*> m_RenderObjects;
    //std::vector<RenderObject*> m_LightingObjects;
    RenderObject* m_LightingObjects;
    std::mutex m_RenderLock;


    void HandleLightSources();
    void CalculateAndSetLightingUniforms();
};

#endif