#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>




class Shader {
public:
    unsigned int ID;
    bool IsValid() const { return m_IsValid; }
    
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void Use() const;
    void SetMat4(const std::string& name, const glm::mat4& matrix) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetInt(const std::string& name, int value) const;
    void SetBool(const std::string& name, bool value) const;
    GLint GetUniformLocation(const std::string& name) const;

private:
    bool m_IsValid = false;
    std::string LoadShaderSource(const char* filePath);
    bool CheckCompileErrors(GLuint shader, std::string type);
};