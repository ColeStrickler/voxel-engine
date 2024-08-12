#include "glbuffer.h"
#include <stdio.h>
#include <nvml.h>


VertexBuffer::VertexBuffer(uint64_t size)
{
    glCreateBuffers(1, &m_BufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(float *data, uint64_t size)
{
    glCreateBuffers(1, &m_BufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_BufferId);
    if (m_Layout)
        delete m_Layout;
    //printf("~VertexBuffer() \n");
}

void VertexBuffer::Delete()
{
    glDeleteBuffers(1, &m_BufferId);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(const void *data, uint64_t size, uint64_t offset)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data); // updates from the beginning of the buffer
}

void VertexBuffer::UnsetData(uint64_t offset, uint64_t size)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    glInvalidateBufferSubData(m_BufferId, offset, size);
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
    //printf("~IndexBuffer()\n");
    glDeleteBuffers(1, &m_BufferId);
}

void IndexBuffer::Delete()
{
    glDeleteBuffers(1, &m_BufferId);
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId);
}

void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

BufferElement::BufferElement(const std::string& Name, ShaderDataType type, bool normalized) :
    m_Offset(0), m_Type(type), m_Size(ShaderDataTypeSize(type)), m_Normalized(normalized), m_Count(ShaderDataTypeCount(type))
{

}

BufferElement::~BufferElement()
{
  //  printf("~BufferElement()\n");
}

BufferLayout::BufferLayout(std::initializer_list<BufferElement*> elements) :
    m_Elements(elements)
{
    InitLayout();    
}

BufferLayout::~BufferLayout()
{
    for (auto& element: m_Elements)
        delete element;
    //printf("~BufferLayout()\n");
}

void BufferLayout::InitLayout()
{
    uint32_t offset = 0;
    m_Stride = 0;
    for (auto& element : m_Elements)
    {
        element->m_Offset = offset;
        offset += element->m_Size;
        m_Stride += element->m_Size;
    }
}
