#include "mesh.h"
#include "material.h"
#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector> // Needed for std::vector
#include <string> // Needed for std::string and std::getline

Mesh::~Mesh() {
    // Keep this cleanup code as is
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

bool Mesh::LoadFromOBJ(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << path << "\n";
        return false;
    }

    // Temporary storage for vertex data
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    
    // Clear member vectors before loading new data
    m_Vertices.clear();
    m_Indices.clear();

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            // Vertex position
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        } else if (prefix == "vt") {
            // Texture coordinate
            glm::vec2 texCoord;
            ss >> texCoord.x >> texCoord.y;
            texCoords.push_back(texCoord);
        } else if (prefix == "vn") {
            // Normal
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (prefix == "f") {
            // Face - handle v/vt/vn format
            std::vector<unsigned int> face_vertex_indices;
            std::vector<unsigned int> face_tex_indices;
            std::vector<unsigned int> face_normal_indices;
            std::string vertex_spec;
            
            while (ss >> vertex_spec) {
                std::stringstream face_ss(vertex_spec);
                std::string segment;
                unsigned int vertex_index = 0, tex_index = 0, normal_index = 0;
                
                // Parse vertex index
                if (std::getline(face_ss, segment, '/')) {
                    if (!segment.empty()) {
                        vertex_index = std::stoul(segment);
                    }
                }
                
                // Parse texture coordinate index
                if (std::getline(face_ss, segment, '/')) {
                    if (!segment.empty()) {
                        tex_index = std::stoul(segment);
                    }
                }
                
                // Parse normal index
                if (std::getline(face_ss, segment, '/')) {
                    if (!segment.empty()) {
                        normal_index = std::stoul(segment);
                    }
                }
                
                face_vertex_indices.push_back(vertex_index - 1); // Convert to 0-based
                face_tex_indices.push_back(tex_index - 1);
                face_normal_indices.push_back(normal_index - 1);
            }

            // Triangulate the face
            if (face_vertex_indices.size() >= 3) {
                // First triangle
                m_Indices.push_back(face_vertex_indices[0]);
                m_Indices.push_back(face_vertex_indices[1]);
                m_Indices.push_back(face_vertex_indices[2]);

                // If it was a quad, create the second triangle
                if (face_vertex_indices.size() == 4) {
                    m_Indices.push_back(face_vertex_indices[0]);
                    m_Indices.push_back(face_vertex_indices[2]);
                    m_Indices.push_back(face_vertex_indices[3]);
                }
            }
        }
    }

    // Create vertices with all attributes
    for (size_t i = 0; i < positions.size(); ++i) {
        Vertex vertex;
        vertex.Position = positions[i];
        
        // Set texture coordinates (if available)
        if (i < texCoords.size()) {
            vertex.TexCoords = texCoords[i];
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        
        // Set normals (if available)
        if (i < normals.size()) {
            vertex.Normal = normals[i];
        } else {
            // Calculate a simple normal if not provided
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        
        m_Vertices.push_back(vertex);
    }

    // Check if we loaded any geometry
    if (m_Vertices.empty() || m_Indices.empty()) {
        std::cerr << "Warning: Mesh data not loaded correctly (Vertices: "
                  << m_Vertices.size() << ", Indices: " << m_Indices.size() << ") from " << path << std::endl;
        glDeleteVertexArrays(1, &m_VAO); m_VAO = 0;
        glDeleteBuffers(1, &m_VBO); m_VBO = 0;
        glDeleteBuffers(1, &m_EBO); m_EBO = 0;
        return false;
    }

    // Create VAO, VBO, EBO and upload data
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

    // Setup vertex attributes
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Texture coordinate attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    
    // Normal attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glBindVertexArray(0);
    
    std::cout << "Loaded mesh: " << path << " (Vertices: " << m_Vertices.size() 
              << ", Indices: " << m_Indices.size() << ")" << std::endl;
    return true;
}

void Mesh::Draw() const {
    // Added a guard condition - good practice
    if (m_VAO == 0 || m_Indices.empty()) {
        // Don't try to draw if loading failed or mesh is empty
        return;
    }

    // Drawing code kept from original
    glBindVertexArray(m_VAO);
    // Use GLsizei cast for size, which is technically more correct for glDrawElements count
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::Draw(Shader* shader, Material* material) const {
    if (m_VAO == 0 || m_Indices.empty()) {
        return;
    }

    // Bind material if provided
    if (material && shader) {
        material->Bind(shader);
    }

    // Draw the mesh
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
