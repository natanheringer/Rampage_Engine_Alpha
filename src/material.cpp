#include "material.h"
#include "shader.h"

Material::Material() : m_Name("Default") {
}

Material::Material(const std::string& name) : m_Name(name) {
}

Material::~Material() {
    // Note: We don't delete textures here as they might be shared
    // Texture cleanup should be handled by the texture manager
}

void Material::SetDiffuseTexture(Texture* texture) {
    m_DiffuseTexture = texture;
}

void Material::SetSpecularTexture(Texture* texture) {
    m_SpecularTexture = texture;
}

void Material::SetNormalTexture(Texture* texture) {
    m_NormalTexture = texture;
}

void Material::Bind(Shader* shader) const {
    if (!shader) return;

    // Set material properties with error checking
    shader->SetVec3("material.ambient", ambient);
    shader->SetVec3("material.diffuse", diffuse);
    shader->SetVec3("material.specular", specular);
    shader->SetFloat("material.shininess", shininess);

    // Bind diffuse texture
    if (m_DiffuseTexture) {
        m_DiffuseTexture->Bind(0);
        shader->SetInt("material.diffuse", 0);
        shader->SetBool("material.hasDiffuseTexture", true);
    } else {
        shader->SetBool("material.hasDiffuseTexture", false);
    }

    // Bind specular texture
    if (m_SpecularTexture) {
        m_SpecularTexture->Bind(1);
        shader->SetInt("material.specular", 1);
        shader->SetBool("material.hasSpecularTexture", true);
    } else {
        shader->SetBool("material.hasSpecularTexture", false);
    }

    // Bind normal texture
    if (m_NormalTexture) {
        m_NormalTexture->Bind(2);
        shader->SetInt("material.normal", 2);
        shader->SetBool("material.hasNormalTexture", true);
    } else {
        shader->SetBool("material.hasNormalTexture", false);
    }
} 