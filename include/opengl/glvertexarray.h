#ifndef GLVERTEXARRAY_H
#define GLVERTEXARRAY_H
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glbuffer.h"


class VertexArray
{
public:
    VertexArray();
    ~VertexArray();
    void Bind();
    void Unbind();
    void AddIndexBuffer(const IndexBuffer& index_buffer);
    void AddVertexBuffer(const VertexBuffer& vertex_buffer);

private:
    unsigned int m_ArrayId;
    //std::vector<VertexBuffer> m_VertexBuffers; --> will want to be able to pass multiple at some point
    VertexBuffer m_VertexBuffer;
    IndexBuffer m_IndexBuffer;
};

#endif