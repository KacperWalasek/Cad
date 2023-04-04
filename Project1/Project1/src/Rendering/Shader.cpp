#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
    : vertexPath(vertexPath), fragmentPath(fragmentPath)
{
}

void Shader::Init()
{
    ID = glCreateProgram(); 
    loadShaderFile(vertexPath, GL_VERTEX_SHADER);
    loadShaderFile(fragmentPath, GL_FRAGMENT_SHADER);
}

void Shader::loadShaderFile(const char* path, GLenum shaderType)
{
    std::string shaderCode;
    std::ifstream shaderFile;

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        shaderFile.open(path);
        std::stringstream shaderStream;

        shaderStream << shaderFile.rdbuf();

        shaderFile.close();

        shaderCode = shaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* shaderCodeChar = shaderCode.c_str();

    unsigned int shaderId;

    shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderCodeChar, NULL);
    glCompileShader(shaderId);
    checkCompileErrors(shaderId, path);
    
    glAttachShader(ID, shaderId);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(shaderId);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

