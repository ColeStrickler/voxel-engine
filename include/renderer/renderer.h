#ifndef RENDERER_H
#define RENDERER_H
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include "renderobject.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void AddRenderObject(RenderObject* obj);
    void RenderAllObjects();
    RenderObject* FindClosestObject(const glm::vec3& pos);
private:
    std::vector<RenderObject*> m_RenderObjects;
};

#endif