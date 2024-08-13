#include "glvertexarray.h"

VertexArray::VertexArray() : m_Count(0)
{
    glCreateVertexArrays(1, &m_ArrayId);
}

VertexArray::~VertexArray()
{
    
    if (m_VertexBuffer)
    {        
        delete m_VertexBuffer;
    }
    if (m_IndexBuffer)
    {
        delete m_IndexBuffer;
    }
    Delete();
    //printf("~VertexArray() --> 0x%x, 0x%x\n", m_VertexBuffer, m_IndexBuffer);
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
    SetCount(m_IndexBuffer->m_Count);
}

void VertexArray::AddVertexBuffer(VertexBuffer* vertex_buffer)
{
    auto& layout = vertex_buffer->GetLayout();
    auto& elements = layout.GetElements();
    m_VertexBuffer = vertex_buffer;
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

void VertexArray::SetCount(unsigned int count)
{
    m_Count = count;
}

unsigned int VertexArray::GetCount()
{
    return m_Count;
}
