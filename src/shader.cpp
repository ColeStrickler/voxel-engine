#include "shader.h"
#include <fstream>
#include <sstream>



Shader::Shader(const std::string &shaderfile, unsigned int shader_type)
{
    m_error = ShaderError::NO_ERROR_OK;
    m_shaderContent = FileToString(shaderfile);
    if (m_error != ShaderError::NO_ERROR_OK)
    {
        m_log = "Shader file unable to be read\n";
        return;
    }
    m_shaderID = glCreateShader(shader_type);

    const char* shader_code = m_shaderContent.c_str();
    glShaderSource(m_shaderID, 1, &shader_code, NULL);
    glCompileShader(m_shaderID);

    /* Make sure compilation was successful */
    int  success;
    char infoLog[512];
    glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        m_error = ShaderError::COMPILE_ERROR;
        glGetShaderInfoLog(m_shaderID, 512, NULL, infoLog);
        m_log = "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" + std::string(infoLog) + "\n";
    }
}

Shader::~Shader()
{
    DumpLog();
    //glDeleteShader(m_shaderID); // should we move this?
}

void Shader::Attach(unsigned int program) const
{
    glAttachShader(program, m_shaderID);
}

std::string Shader::FetchLog() const
{
    return m_log;
}

ShaderError Shader::CheckError() const
{
    return m_error;
}

void Shader::DumpLog()
{
    // set size to zero
    m_log.clear();
    // attempt to shrink backing memory to match current size
    m_log.shrink_to_fit();
}

void Shader::Delete() const
{
    glDeleteShader(m_shaderID); 
}

std::string Shader::FileToString(const std::string &FilePath)
{
    std::ifstream filestream(FilePath);

    if (!filestream.is_open())
    {
        m_error = ShaderError::FILE_OPEN_ERROR;
        return "";
    }

    std::ostringstream file_content;
    file_content << filestream.rdbuf();
    filestream.close();
    return file_content.str();
}

ShaderProgram::ShaderProgram()
{
    m_ProgramId = glCreateProgram();
   
}

ShaderProgram::~ShaderProgram()
{
    //glDeleteProgram(m_ProgramId);
}

void ShaderProgram::AddShader(Shader *shader)
{
    m_shaders.push_back(shader);
}

void ShaderProgram::Bind() const
{
     glUseProgram(m_ProgramId);
}

/*
    Call after adding your shaders to the program
*/
bool ShaderProgram::Compile()
{
    /*
        attach the shaders for linking

        If the shaders are no longer in use, they can now be deleted
    */
    for (auto& shader: m_shaders)
    {
        shader->Attach(m_ProgramId);
    }

    int success;
    char infoLog[512];
    glLinkProgram(m_ProgramId);
    // check for linking errors
    glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_ProgramId, 512, NULL, infoLog);
        m_log = "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(infoLog);
        m_error = ShaderError::PROGRAM_LINK_ERROR;
        return false;
    }
    return true;
}

void ShaderProgram::Delete() const
{
    glDeleteProgram(m_ProgramId);
}

std::string ShaderProgram::FetchLog() const
{
    return m_log;
}

ShaderError ShaderProgram::CheckError() const
{
    return m_error;
}

void ShaderProgram::DumpLog()
{
    m_log.clear();
    m_log.shrink_to_fit();
}
