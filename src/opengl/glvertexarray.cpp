#include "glvertexarray.h"

VertexArray::VertexArray()
{
    glCreateVertexArrays(1, &m_ArrayId);
}

VertexArray::~VertexArray()
{
   // 
}

void VertexArray::Delete()
{
    glDeleteVertexArrays(1, &m_ArrayId);
}

void VertexArray::Bind()
{
    glBindVertexArray(m_ArrayId);
}

void VertexArray::Unbind()
{
    glBindVertexArray(0);
}

void VertexArray::AddIndexBuffer(IndexBuffer* index_buffer)
{
    this->Bind();
    index_buffer->Bind();
    m_IndexBuffer = index_buffer;
}

void VertexArray::AddVertexBuffer(VertexBuffer* vertex_buffer)
{
    auto& layout = vertex_buffer->GetLayout();
    auto& elements = layout.GetElements();
    assert(elements.size() > 0);
    this->Bind();
    vertex_buffer->Bind();
    unsigned int offset = 0;
    unsigned int index = 0;
    for (auto& element: elements)
    {
        glEnableVertexAttribArray(index); // must enable the index for it to be used
        glVertexAttribPointer(index, element->GetCount(), ShaderDataTypeToGLEnum(element->GetType()), element->m_Normalized, layout.GetStride(), (const void*)offset);
        offset += element->m_Size;
        index++;
    }
    
}
