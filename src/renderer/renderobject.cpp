#include "renderobject.h"

RenderObject::RenderObject(VertexArray* va, VertexBuffer* vb,  ShaderProgram* sp, IndexBuffer* ib) : m_VertexArray(va), m_VertexBuffer(vb), \
    m_ShaderProgram(sp), m_IndexBuffer(ib), m_Model(glm::mat4(1.0f)), m_Position(glm::vec3(0.0f)), m_bWireFrame(false)
{
    m_bUseIndexBuffer = true;
    printf("RenderObject::RenderObject()\n");
}

RenderObject::RenderObject(VertexArray* va, VertexBuffer* vb, ShaderProgram* sp) : m_VertexArray(va), m_VertexBuffer(vb), m_ShaderProgram(sp), \
    m_Model(glm::mat4(1.0f)), m_Position(glm::vec3(0.0f)), m_bWireFrame(false)
{
    m_bUseIndexBuffer = false;
     printf("RenderObject::RenderObject()\n");
}

RenderObject::~RenderObject()
{
}

void RenderObject::Render()
{
    //printf("RenderObject::render()! %x\n", m_ShaderProgram);
    m_ShaderProgram->Bind();
    m_VertexArray->Bind();
    //printf("Bound!\n");

    /*
        We will eventually want to have various shading models that we will switch off off here based on the object type

        For now we content ourselves with throwing in the model matrix
    */

    m_ShaderProgram->SetUniformMat4("model", m_Model);

    DrawCall();
}

void RenderObject::SetPosition(const glm::vec3 &position)
{
    m_Position = position;
    m_Model = glm::translate(glm::mat4(1.0f), m_Position);
}

void RenderObject::Translate(const glm::vec3 &translation_vec)
{
    m_Position += translation_vec;
    m_Model = glm::translate(m_Model, translation_vec);
}

void RenderObject::Rotate(const glm::vec3 &rotation_axis, float angle)
{
    m_Model = glm::rotate(m_Model, angle, rotation_axis);
}

void RenderObject::DrawCall() const
{
    int count = m_VertexArray->GetCount();
    if (m_bUseIndexBuffer)
    {
        if (m_bWireFrame)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, nullptr);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        if (m_bWireFrame)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}
