#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H
#include "glvertexarray.h"
#include "glbuffer.h"
#include "shader.h"


enum OBJECTYPE
{
    LightSource,
    Regular
};





class RenderObject
{
public:
    RenderObject(VertexArray* va, VertexBuffer* vb, ShaderProgram* sp, IndexBuffer* ib);
    RenderObject(VertexArray* va, VertexBuffer* vb, ShaderProgram* sp);
    ~RenderObject();
    void Render();

    const glm::vec3& GetPosition() const {return m_Position;}
    void SetPosition(const glm::vec3& position);
    void Translate(const glm::vec3& translation_vec);
    void Rotate(const glm::vec3& rotation_axis, float angle);
    void ToggleWireFrame() {m_bWireFrame = !m_bWireFrame;}
    void DrawCall() const;
private:
    bool m_bUseIndexBuffer;
    bool m_bWireFrame;
    VertexArray* m_VertexArray;
    VertexBuffer* m_VertexBuffer;
    IndexBuffer* m_IndexBuffer;
    ShaderProgram* m_ShaderProgram;
    glm::vec3 m_Position;
    glm::mat4 m_Model;
};










#endif