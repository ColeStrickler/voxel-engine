#include "glbuffer.h"
#include <stdio.h>

VertexBuffer::VertexBuffer(uint32_t size)
{
    glCreateBuffers(1, &m_BufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(float *data, uint32_t size)
{
    printf("0\n");
    glCreateBuffers(1, &m_BufferId);
    printf("1\n");
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    printf("2\n");
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    printf("3\n");
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_BufferId);
}

void VertexBuffer::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
}

void VertexBuffer::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(const void *data, uint32_t size)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data); // updates from the beginning of the buffer
}

IndexBuffer::IndexBuffer(uint32_t *indices, uint32_t count) : m_Count(count)
{
    glCreateBuffers(1, &m_BufferId);

    // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound Vertex Array Object
    // Binding with GL_ARRAY_BUFFER allows this to always be loaded
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_BufferId);
}

void IndexBuffer::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId);
}

void IndexBuffer::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
