#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>




class Shader {
public:
    unsigned int ID;
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void Use() const;
    void SetMat4(const std::string& name, const glm::mat4& matrix) const;

private:
    std::string LoadShaderSource(const char* filePath);
    void CheckCompileErrors(GLuint shader, std::string type);
};