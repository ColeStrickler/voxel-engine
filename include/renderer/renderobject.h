#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H
#include "model_loader.h"
#include "glvertexarray.h"
#include "glbuffer.h"
#include "shader.h"
#include "material.h"
#include "model_loader.h"
#include "profiler.h"

#define STENCIL_OUTLINE_SCALE_FACTOR (1.20f)
#define STENCIL_OUTLINE_INV_SCALE_FACTOR (1.0f/STENCIL_OUTLINE_SCALE_FACTOR)

enum OBJECTYPE
{
    RegularMaterial,
    PointLightSource,
    TexturedObject,
    DirectionalLightSource,
    TextObject,
    ComplexModelObject,
    OutlineObject,
    ChunkMesh
};



class RenderObject
{
public:
    RenderObject(VertexArray* va, VertexBuffer* vb, ShaderProgram* sp, IndexBuffer* ib, OBJECTYPE type = OBJECTYPE::RegularMaterial);
    RenderObject(VertexArray* va, VertexBuffer* vb, ShaderProgram* sp, OBJECTYPE type = OBJECTYPE::RegularMaterial);
    RenderObject(ShaderProgram* sp, MeshModel* model);
    ~RenderObject();
    bool Render();
    RenderObject* Duplicate();
    
    const glm::vec3& GetPosition() const {return m_Position;}
    void SetPosition(const glm::vec3& position);
    void Translate(const glm::vec3& translation_vec);
    void Rotate(const glm::vec3& rotation_axis, float angle);
    void Scale(float scale);
    void DrawCall() const;
    ShaderProgram* GetShaderProgram() const {return m_ShaderProgram;}
    OBJECTYPE GetType() const {return m_ObjectType;}
    // may wanna have two different renderobject types and lists once we get multiple light sources
    glm::vec3 m_Position;
    //glm::vec3 m_LightColor;
    

    Light m_Light;
    Material m_Material;
    MeshModel* m_MeshModel;
  //  std::vector<RenderObject*> m_SubObjects;
    
    
    TextureObject m_TexturedObject;
    void SetSpecularMap(Texture* sm) {m_TexturedObject.AddSpecularMap(sm);}
    void SetDiffuseMap(Texture* dm) {m_TexturedObject.AddDiffuseMap(dm);}

    int m_MaterialId;


    void ToggleWireFrame() { m_bWireFrame = !m_bWireFrame; }
    bool UsingWireframe() const { return m_bWireFrame; }
    void ToggleStencilOutline() { m_bStencilOutline = !m_bStencilOutline;}
    bool UsingStencilOutline() const { return m_bStencilOutline; }
    bool m_bDelete;
    OBJECTYPE m_ObjectType;
private:
    /* Handle Shader Uniforms*/
    
    void SetShaders();
    void HandlePhongShaders();

    


    bool m_bUseIndexBuffer;
    bool m_bWireFrame;
    bool m_bStencilOutline;
    VertexArray* m_VertexArray;
    VertexBuffer* m_VertexBuffer;
    IndexBuffer* m_IndexBuffer;
    ShaderProgram* m_ShaderProgram;
    
    glm::mat4 m_Model;
};




struct WireFrame_RAII
{
    WireFrame_RAII(bool use) : m_Use(use)
    {
        if (m_Use)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    ~WireFrame_RAII()
    {
        if (m_Use)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    bool m_Use;
};


struct StencilOutline_RAII
{
    StencilOutline_RAII(bool use, RenderObject* obj);

    ~StencilOutline_RAII();
    RenderObject* m_Obj;
    bool m_Use;
};






#endif