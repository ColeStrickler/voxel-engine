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

enum ShaderError
{
    NO_ERROR_OK,
    FILE_OPEN_ERROR,
    COMPILE_ERROR,
    PROGRAM_LINK_ERROR
};


class Shader
{
public:
    Shader(const std::string& shaderfile, unsigned int shader_type);
    ~Shader();
    void Attach(unsigned int program) const;
    std::string FetchLog() const;
    ShaderError CheckError() const;
    void DumpLog();
    void Delete() const;

private:
    std::string FileToString(const std::string& FilePath);
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
private:
   unsigned int m_ProgramId;
    ShaderError m_error;
    std::string m_log;
    std::vector<Shader*> m_shaders;
};


#endif

