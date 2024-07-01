#include "renderobject.h"
#include "renderer.h"
#include "logger.h"
extern Renderer renderer;
extern GLManager gl;
extern Logger logger;
RenderObject::RenderObject(VertexArray* va, VertexBuffer* vb,  ShaderProgram* sp, IndexBuffer* ib, OBJECTYPE type) : m_VertexArray(va), m_VertexBuffer(vb), \
    m_ShaderProgram(sp), m_IndexBuffer(ib), m_Model(glm::mat4(1.0f)), m_Position(glm::vec3(0.0f)), m_bWireFrame(false), m_ObjectType(type)
{
    if (ib != nullptr)
        m_bUseIndexBuffer = true;
    
}

RenderObject::RenderObject(VertexArray* va, VertexBuffer* vb, ShaderProgram* sp, OBJECTYPE type ) : m_VertexArray(va), m_VertexBuffer(vb), m_ShaderProgram(sp), \
    m_Model(glm::mat4(1.0f)), m_Position(glm::vec3(0.0f)), m_bWireFrame(false), m_ObjectType(type)
{
    m_bUseIndexBuffer = false;
    m_Light.color = glm::vec3(1.0f, 1.0f, 1.0f);
}

RenderObject::~RenderObject()
{

}

void RenderObject::Render()
{

    m_ShaderProgram->Bind();
    m_VertexArray->Bind();


    // this could be placed much better for optimization

    /*
        We will eventually want to have various shading models that we will switch off off here based on the object type

        For now we content ourselves with throwing in the model matrix
    */
    SetShaders();
    

    DrawCall();
}

RenderObject* RenderObject::Duplicate()
{
    RenderObject* new_obj = new RenderObject(*this);
    new_obj->SetPosition(GetPosition());
    logger.Log(LOGTYPE::INFO, "RenderObject::Duplicate() --> duplicated object.");
    return new_obj;
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

void RenderObject::SetShaders()
{
    /* We put shaders common to all objects here */
    m_ShaderProgram->SetUniformMat4("model", m_Model);
    if(!gl.UpdateCameraMVP(m_ShaderProgram))
        logger.Log(LOGTYPE::ERROR, "RenderObject::SetShaders() --> UpdateCameraMVP() failed\n");

    switch(renderer.GetLightingModel())
    {
        case LightingModel::Phong: HandlePhongShaders(); break;
        default: break;
    }
    return;
}

void RenderObject::HandlePhongShaders()
{
    switch(m_ObjectType)
    {
        case OBJECTYPE::LightSource:
        {
            printf("set light color %.2f, %.2f, %.2f\n", m_Light.color.x, m_Light.color.y, m_Light.color.z);
            m_ShaderProgram->SetUniformVec3("lightColor", m_Light.color);
            break;
        }
        case OBJECTYPE::Regular:
        {
            // this is patchwork for now
            // need to clean this up
            auto camer_pos = gl.GetCamera()->GetPosition();
            m_ShaderProgram->SetUniformVec3("viewPos", camer_pos);


            auto light_obj = renderer.GetLighting();
            if (light_obj)
            {
                auto& light = light_obj->m_Light;
                //m_ShaderProgram->SetUniformVec3("light.color", light_obj->m_LightColor);
                m_ShaderProgram->SetUniformVec3("light.position", light.position);
                m_ShaderProgram->SetUniformVec3("light.diffuse", light.diffuse);
                m_ShaderProgram->SetUniformVec3("light.specular", light.specular);
                m_ShaderProgram->SetUniformVec3("light.ambient", light.ambient);
            }
            

            m_ShaderProgram->SetUniformVec3("material.ambient", m_Material.ambient);
            m_ShaderProgram->SetUniformVec3("material.diffuse", m_Material.diffuse);
            m_ShaderProgram->SetUniformVec3("material.specular", m_Material.specular);
            m_ShaderProgram->SetUniform1f("material.shininess", m_Material.shininess);
            m_ShaderProgram->SetUniformVec3("material.color", m_Material.color);
            break;
        }
    }
}