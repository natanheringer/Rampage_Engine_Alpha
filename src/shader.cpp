#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include <glad/glad.h> // Para OpenGL
#include <fstream>
#include <sstream>
#include <iostream>



Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    const std::string vertexCode = LoadShaderSource(vertexPath.c_str());
    const std::string fragmentCode = LoadShaderSource(fragmentPath.c_str());

    // Check if shader files were loaded successfully
    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cerr << "Failed to load shader files. Shader compilation aborted." << std::endl;
        m_IsValid = false;
        return;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile Vertex Shader
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    bool vertexSuccess = CheckCompileErrors(vertex, "VERTEX");

    // Compile Fragment Shader
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    bool fragmentSuccess = CheckCompileErrors(fragment, "FRAGMENT");

    // Link shaders into program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    bool linkSuccess = CheckCompileErrors(ID, "PROGRAM");

    // Delete individual shaders after linking
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Set validity based on compilation and linking success
    m_IsValid = vertexSuccess && fragmentSuccess && linkSuccess;
    
    if (m_IsValid) {
        std::cout << "Shader compiled successfully: " << vertexPath << ", " << fragmentPath << std::endl;
    } else {
        std::cerr << "Shader compilation failed: " << vertexPath << ", " << fragmentPath << std::endl;
    }
}

void Shader::Use() const {
    glUseProgram(ID);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    } else {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
    }
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    } else {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
    }
}

void Shader::SetFloat(const std::string& name, float value) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1f(location, value);
    } else {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
    }
}

void Shader::SetInt(const std::string& name, int value) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1i(location, value);
    } else {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
    }
}

void Shader::SetBool(const std::string& name, bool value) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1i(location, (int)value);
    } else {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
    }
}

GLint Shader::GetUniformLocation(const std::string& name) const {
    return glGetUniformLocation(ID, name.c_str());
}

std::string Shader::LoadShaderSource(const char* filePath)
{
    std::ifstream file(filePath);
    std::stringstream buffer;

    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o shader: " << filePath << std::endl;
        return "";
    }

    buffer << file.rdbuf();
    return buffer.str();
}

bool Shader::CheckCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "Shader compilation error (" << type << "):\n" << infoLog << std::endl;
            return false;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "Shader program linking error:\n" << infoLog << std::endl;
            return false;
        }
    }
    return true;
}
