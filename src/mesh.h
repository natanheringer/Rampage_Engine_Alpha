#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
};

class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    bool LoadFromOBJ(const std::string& path);
    void Draw() const;
    void Draw(class Shader* shader, class Material* material) const;

private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;

    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLuint m_EBO = 0;
};