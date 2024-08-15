#ifndef GLBUFFER_H
#define GLBUFFER_H
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

enum ShaderDataType
{
    None = 0, 
    Float, 
    Float2, 
    Float3, 
    Float4, 
    Mat3, 
    Mat4, 
    Int, 
    Int2, 
    Int3, 
    Int4, 
    Bool,
    UnsignedByte
};


static uint32_t ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 1;
        case ShaderDataType::UnsignedByte: return 1;
	}
	return 0;
}

static uint32_t ShaderDataTypeCount(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Float:    return 1;
		case ShaderDataType::Float2:   return 2;
		case ShaderDataType::Float3:   return 3;
		case ShaderDataType::Float4:   return 4;
		case ShaderDataType::Mat3:     return 3;
		case ShaderDataType::Mat4:     return 4;
		case ShaderDataType::Int:      return 1;
		case ShaderDataType::Int2:     return 2;
		case ShaderDataType::Int3:     return 3;
		case ShaderDataType::Int4:     return 4;
		case ShaderDataType::Bool:     return 1;
	}
	return 0;
}

static GLenum ShaderDataTypeToGLEnum(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
	}
    printf("type %d", type);
    assert(false);
	return 0;
}



class BufferElement
{
public:
    BufferElement(const std::string& Name, ShaderDataType type, bool normalized);
    ~BufferElement();
    //std::string GetName() const {return m_Name;}
    ShaderDataType GetType() const {return m_Type;}
    const unsigned int& GetCount() const {return m_Count;}
    uint32_t m_Size;
    uint32_t m_Offset; // offset within stride
    bool m_Normalized;
    unsigned int m_Count;
private:
    ShaderDataType m_Type;
};


class BufferLayout
{
public:
    BufferLayout() = default;
    BufferLayout(std::initializer_list<BufferElement*> elements);
    ~BufferLayout();
    const std::vector<BufferElement*>& GetElements() const {return m_Elements;}
    unsigned int GetStride() const {return m_Stride;}
private:
    void InitLayout();
    std::vector<BufferElement*> m_Elements;
    unsigned int m_Stride;
};



class VertexBuffer
{
public:
    VertexBuffer() = default;
    VertexBuffer(uint64_t size);
    VertexBuffer(float* data, uint64_t size);
    ~VertexBuffer();
    void Grow(uint64_t newSize, uint64_t oldSize);
    void Delete();
    void Bind() const;
    void Unbind() const;
    void SetData(const void* data, uint64_t offset, uint64_t size);
    void UnsetData(uint64_t offset, uint64_t size);
    void SetLayout(BufferLayout* layout) {m_Layout = layout;};
    const BufferLayout& GetLayout() const {return *m_Layout;}
private:
    unsigned int m_BufferId;
    BufferLayout* m_Layout;
};


class IndexBuffer
{
public:
    IndexBuffer() = default;
    IndexBuffer(uint32_t* indices, uint32_t count);
    ~IndexBuffer();
    void Delete();
    void Bind() const;
    void Unbind() const;
    uint32_t m_Count;
private:
    unsigned int m_BufferId;
    
};





#endif