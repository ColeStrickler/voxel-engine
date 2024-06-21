#ifndef SHADER_H
#define SHADER_H
#include <string>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>


enum ShaderError
{
    NO_ERROR_OK,
    FILE_OPEN_ERROR,
    COMPILE_ERROR,
    PROGRAM_LINK_ERROR,
    INVALID_SHADER_TYPE_ERROR,
    UNIFORM_SET_ERROR
};




class Shader
{
public:
    Shader(const std::string& shaderfile, GLenum shader_type);
    ~Shader();
    void Attach(unsigned int program) const;
    std::string FetchLog() const;
    ShaderError CheckError() const;
    void DumpLog();
    void Delete() const;
    GLenum GetType() const {return m_ShaderType;}
private:
    std::string FileToString(const std::string& FilePath);
    GLenum m_ShaderType;
    unsigned int m_shaderID;
    std::string m_shaderContent;
    ShaderError m_error;
    std::string m_log;
    
};


class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();
    void AddShader(Shader* shader);
    void Bind() const;
    bool Compile();
    void Delete() const;
    std::string FetchLog() const;
    ShaderError CheckError() const;
    void DumpLog();
     bool SetUniform1f(const std::string& name, float data);
private:
    int GetUniformLocation(const std::string& UniformName);
   
    int ShaderTypeIndex(Shader* shader);

    std::unordered_map<std::string, int> m_UniformCache;
    unsigned int m_ProgramId;
    ShaderError m_error;
    std::string m_log;
    std::vector<Shader*> m_shaders;
};


#endif

