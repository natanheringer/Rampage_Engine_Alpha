#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include "texture.h"

class Material {
public:
    Material();
    Material(const std::string& name);
    ~Material();

    // Material properties
    glm::vec3 ambient = glm::vec3(0.1f);
    glm::vec3 diffuse = glm::vec3(0.7f);
    glm::vec3 specular = glm::vec3(1.0f);
    float shininess = 32.0f;

    // Texture management
    void SetDiffuseTexture(Texture* texture);
    void SetSpecularTexture(Texture* texture);
    void SetNormalTexture(Texture* texture);
    
    Texture* GetDiffuseTexture() const { return m_DiffuseTexture; }
    Texture* GetSpecularTexture() const { return m_SpecularTexture; }
    Texture* GetNormalTexture() const { return m_NormalTexture; }

    // Bind material to shader
    void Bind(class Shader* shader) const;
    
    // Get material name
    const std::string& GetName() const { return m_Name; }

private:
    std::string m_Name;
    Texture* m_DiffuseTexture = nullptr;
    Texture* m_SpecularTexture = nullptr;
    Texture* m_NormalTexture = nullptr;
}; 