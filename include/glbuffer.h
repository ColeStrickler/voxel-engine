#ifndef GLBUFFER_H
#define GLBUFFER_H
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class VertexBuffer
{
public:
    VertexBuffer(uint32_t size);
    VertexBuffer(float* data, uint32_t size);
    ~VertexBuffer();
    void Bind();
    void Unbind();
    void SetData(const void* data, uint32_t size);
private:
    unsigned int m_BufferId;

};


class IndexBuffer
{
public:
    IndexBuffer(uint32_t* indices, uint32_t count);
    ~IndexBuffer();
    void Bind();
    void Unbind();
private:
    unsigned int m_BufferId;
    uint32_t m_Count;
};



#endif