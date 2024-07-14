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
   // m_Light.color = glm::vec3(1.0f, 1.0f, 1.0f);
}

RenderObject::RenderObject(ShaderProgram *sp, MeshModel *model) : m_ObjectType(OBJECTYPE::ComplexModelObject), m_VertexArray(NULL), m_VertexBuffer(NULL),\
    m_IndexBuffer(NULL), m_bUseIndexBuffer(false), m_Model(glm::mat4(1.0f)), m_Position(glm::vec3(0.0f)), m_bWireFrame(false)
{
    m_MeshModel = model;
    m_ShaderProgram =sp;
}

RenderObject::~RenderObject()
{

}

void RenderObject::Render()
{
    
    m_ShaderProgram->Bind();
    if (m_VertexArray != nullptr)
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
    if (m_ObjectType == OBJECTYPE::ComplexModelObject)
    {
        if (m_bWireFrame)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            m_MeshModel->Render(renderer.GetLightingModel(), m_ShaderProgram);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
             m_MeshModel->Render(renderer.GetLightingModel(), m_ShaderProgram);
       
        return;
    }




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


void printVec(const glm::vec3& vec)
{
    printf("%.2f, %.2f, %.2f\n", vec.x, vec.y, vec.z);
}

void RenderObject::HandlePhongShaders()
{
   // Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 0.2f);
    switch(m_ObjectType)
    {
        case OBJECTYPE::PointLightSource:
        {
            m_ShaderProgram->SetUniform1i("ObjectType", m_ObjectType);
           // m_ShaderProgram->SetUniformVec3("lightColor", m_Light.ambient + m_Light.diffuse);
            break;
        }
        case OBJECTYPE::RegularMaterial:
        {
            // this is patchwork for now
            // need to clean this up
            auto camera_pos = gl.GetCamera()->GetPosition();
            m_ShaderProgram->SetUniformVec3("viewPos", camera_pos);
            m_ShaderProgram->SetUniform1i("ObjectType", m_ObjectType);
            // lighting uniforms should already be set because we use the same shader for both light sources and other objects
            m_ShaderProgram->SetUniformVec3("material.ambient", m_Material.ambient);
            m_ShaderProgram->SetUniformVec3("material.diffuse", m_Material.diffuse);
            m_ShaderProgram->SetUniformVec3("material.specular", m_Material.specular);
            m_ShaderProgram->SetUniform1f("material.shininess", m_Material.shininess);
            break;
        }
        case OBJECTYPE::TexturedObject:
        {
            
            auto camera_pos = gl.GetCamera()->GetPosition();
            m_ShaderProgram->SetUniformVec3("viewPos", camera_pos);
            m_ShaderProgram->SetUniform1i("ObjectType", m_ObjectType);
            // lighting uniforms should already be set because we use the same shader for both light sources and other objects

            m_TexturedObject.Bind();
            m_ShaderProgram->SetUniform1f("textureObject.shininess", m_TexturedObject.Shininess);
            m_ShaderProgram->SetUniform1i("textureObject.diffuseMap", m_TexturedObject.GetDiffuseSlot());
            m_ShaderProgram->SetUniform1i("textureObject.specularMap", m_TexturedObject.GetSpecularSlot());
        }
        case OBJECTYPE::ComplexModelObject:
        {
            m_ShaderProgram->SetUniform1i("ObjectType", m_ObjectType);
            break; // rest  set in draw call
        }
        default:
            break;
    }
}
