#include "renderobject.h"
#include "renderer.h"
#include "logger.h"
#include "profiler.h"
extern Renderer renderer;
extern GLManager gl;
extern Logger logger;
RenderObject::RenderObject(VertexArray* va, VertexBuffer* vb,  ShaderProgram* sp, IndexBuffer* ib, OBJECTYPE type) : m_VertexArray(va), m_VertexBuffer(vb), \
    m_ShaderProgram(sp), m_IndexBuffer(ib), m_Model(glm::mat4(1.0f)), m_Position(glm::vec3(0.0f)), m_bWireFrame(false), m_ObjectType(type), m_bStencilOutline(false),
    m_bDelete(false)
{
    if (ib != nullptr)
        m_bUseIndexBuffer = true;
    
}

RenderObject::RenderObject(VertexArray* va, VertexBuffer* vb, ShaderProgram* sp, OBJECTYPE type ) : m_VertexArray(va), m_VertexBuffer(vb), m_ShaderProgram(sp), \
    m_Model(glm::mat4(1.0f)), m_Position(glm::vec3(0.0f)), m_bWireFrame(false), m_ObjectType(type), m_bStencilOutline(false), m_bDelete(false)
{
    m_bUseIndexBuffer = false;
   // m_Light.color = glm::vec3(1.0f, 1.0f, 1.0f);
}

RenderObject::RenderObject(ShaderProgram *sp, MeshModel *model) : m_ObjectType(OBJECTYPE::ComplexModelObject), m_VertexArray(NULL), m_VertexBuffer(NULL),\
    m_IndexBuffer(NULL), m_bUseIndexBuffer(false), m_Model(glm::mat4(1.0f)), m_Position(glm::vec3(0.0f)), m_bWireFrame(false), m_bStencilOutline(false),\
     m_bDelete(false)
{
    m_MeshModel = model;
    m_ShaderProgram =sp;
}

RenderObject::~RenderObject()
{
    delete m_VertexArray;
}

bool RenderObject::Render()
{
    if (m_bDelete)
        return true;
    
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
    return false;
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
    m_Model[3] = glm::vec4(m_Position, 1.0f);
}

void RenderObject::Translate(const glm::vec3 &translation_vec)
{
    m_Position += translation_vec;
    m_Model = glm::translate(m_Model, translation_vec);
}

void RenderObject::Rotate(const glm::vec3 &rotation_axis, float angle)
{
    if (rotation_axis.x == 0.0f && rotation_axis.y == 0.0f && rotation_axis.z == 0.0f)
        return;
    m_Model = glm::rotate(m_Model, angle, rotation_axis);
}

void RenderObject::Scale(float scale)
{
    m_Model = glm::scale(m_Model, glm::vec3(scale, scale, scale));
}

void RenderObject::DrawCall() const
{
    WireFrame_RAII wireframe(m_bWireFrame);
    

    if (m_ObjectType == OBJECTYPE::ComplexModelObject)
    {
        m_MeshModel->Render(renderer.GetLightingModel(), m_ShaderProgram);
        return;
    }
    StencilOutline_RAII stencil(m_bStencilOutline, (RenderObject*)this);
    int count = m_VertexArray->GetCount();
    if (m_bUseIndexBuffer)
    {
       // printf("Count %d\n", count);
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, 36);
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
            m_ShaderProgram->SetUniform1i("textureObject.useDiffuse", 1);
            m_ShaderProgram->SetUniform1i("textureObject.useSpecular", 0);
        }
        case OBJECTYPE::ComplexModelObject:
        {
            m_ShaderProgram->SetUniform1i("ObjectType", m_ObjectType);
            break; // rest  set in draw call
        }
        case OBJECTYPE::OutlineObject:
        {
            m_ShaderProgram->SetUniform1i("ObjectType", m_ObjectType);
            break;
        }
        case OBJECTYPE::ChunkMesh:
        {
            auto camera_pos = gl.GetCamera()->GetPosition();
            m_ShaderProgram->SetUniformVec3("viewPos", camera_pos);
            m_ShaderProgram->SetUniform1i("ObjectType", m_ObjectType);
            m_TexturedObject.Bind();
            m_ShaderProgram->SetUniform1f("textureObject.shininess", m_TexturedObject.Shininess);
            m_ShaderProgram->SetUniform1i("textureObject.diffuseMap", m_TexturedObject.GetDiffuseSlot());
            m_ShaderProgram->SetUniform1i("textureObject.specularMap", m_TexturedObject.GetSpecularSlot());
            m_ShaderProgram->SetUniform1i("textureObject.useDiffuse", 1);
            m_ShaderProgram->SetUniform1i("textureObject.useSpecular", 0);
            break;
        }
        default:
            break;
    }
}

StencilOutline_RAII::StencilOutline_RAII(bool use, RenderObject *obj) : m_Use(use), m_Obj(obj)
{
    if (!m_Use)
    {
        glStencilMask(0x00);
        return;
    }
    else
    {
        glStencilFunc(GL_ALWAYS, 1, 0xFF); 
        glStencilMask(0xFF); 
    }
}

StencilOutline_RAII::~StencilOutline_RAII()
{
    {
        if (!m_Use)
        {
            return;
        }
        else
        {
            /*
                This will scale up the object to draw the outline
            */
            auto type = m_Obj->m_ObjectType;
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00); 
            //glDisable(GL_DEPTH_TEST);
            m_Obj->ToggleStencilOutline();
            m_Obj->Scale(STENCIL_OUTLINE_SCALE_FACTOR);
            m_Obj->m_ObjectType = OBJECTYPE::OutlineObject;
            m_Obj->Render();
            m_Obj->m_ObjectType = type;
            m_Obj->Scale(STENCIL_OUTLINE_INV_SCALE_FACTOR);
            m_Obj->ToggleStencilOutline();
            glStencilMask(0xff); 
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glEnable(GL_DEPTH_TEST);  
        }
    }
}
