#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H
#include "glvertexarray.h"
#include "glbuffer.h"
#include "shader.h"
#include "material.h"

enum OBJECTYPE
{
    RegularMaterial,
    PointLightSource,
    TexturedObject,
    DirectionalLightSource,
    TextObject,
    ComplexModelObject
};






class RenderObject
{
public:
    RenderObject(VertexArray* va, VertexBuffer* vb, ShaderProgram* sp, IndexBuffer* ib, OBJECTYPE type = OBJECTYPE::RegularMaterial);
    RenderObject(VertexArray* va, VertexBuffer* vb, ShaderProgram* sp, OBJECTYPE type = OBJECTYPE::RegularMaterial);
    ~RenderObject();
    void Render();
    RenderObject* Duplicate();
    const glm::vec3& GetPosition() const {return m_Position;}
    void SetPosition(const glm::vec3& position);
    void Translate(const glm::vec3& translation_vec);
    void Rotate(const glm::vec3& rotation_axis, float angle);
    void ToggleWireFrame() {m_bWireFrame = !m_bWireFrame;}
    void DrawCall() const;
    ShaderProgram* GetShaderProgram() const {return m_ShaderProgram;}
    OBJECTYPE GetType() const {return m_ObjectType;}
    // may wanna have two different renderobject types and lists once we get multiple light sources
    glm::vec3 m_Position;
    glm::vec3 m_LightColor;
    Light m_Light;
    Material m_Material;
  //  std::vector<RenderObject*> m_SubObjects;
    
    
    TextureObject m_TexturedObject;
    void SetSpecularMap(Texture* sm) {m_TexturedObject.AddSpecularMap(sm);}
    void SetDiffuseMap(Texture* dm) {m_TexturedObject.AddDiffuseMap(dm);}

    int m_MaterialId;



    
private:
    /* Handle Shader Uniforms*/
    OBJECTYPE m_ObjectType;
    void SetShaders();
    void HandlePhongShaders();




    bool m_bUseIndexBuffer;
    bool m_bWireFrame;
    VertexArray* m_VertexArray;
    VertexBuffer* m_VertexBuffer;
    IndexBuffer* m_IndexBuffer;
    ShaderProgram* m_ShaderProgram;
    
    glm::mat4 m_Model;
};










#endif