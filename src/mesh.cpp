#include "mesh.h"
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

    // Store vertex positions temporarily as in your original code
    std::vector<glm::vec3> positions;
    // Clear member vectors before loading new data
    m_Vertices.clear();
    m_Indices.clear();

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos); // Store positions read
        } else if (prefix == "f") {
            // --- MODIFICATION START ---
            // This block is replaced to handle v/vt/vn and quads

            std::vector<unsigned int> face_vertex_indices; // Temp store for this face's vertex indices
            std::string vertex_spec; // To read "v/vt/vn" chunks
            unsigned int vertex_index; // Extracted vertex index

            // Read all vertex specifications (like "1/1/1", "5/2/1", etc.) from the line
            while (ss >> vertex_spec) {
                std::stringstream face_ss(vertex_spec);
                std::string segment;

                // Get the part before the first '/' (the vertex index string)
                if (std::getline(face_ss, segment, '/')) {
                    // Convert string to unsigned int, OBJ indices are 1-based
                    vertex_index = std::stoul(segment);
                    face_vertex_indices.push_back(vertex_index - 1); // Store the 0-based index
                }
                 // Ignore the rest (vt, vn parts) for now
            }

            // Triangulate the face (handle quads specifically)
            if (face_vertex_indices.size() >= 3) {
                // First triangle (works for triangles and quads)
                m_Indices.push_back(face_vertex_indices[0]);
                m_Indices.push_back(face_vertex_indices[1]);
                m_Indices.push_back(face_vertex_indices[2]);

                // If it was a quad (or more complex polygon starting with a quad), create the second triangle
                if (face_vertex_indices.size() == 4) {
                    m_Indices.push_back(face_vertex_indices[0]); // v1
                    m_Indices.push_back(face_vertex_indices[2]); // v3
                    m_Indices.push_back(face_vertex_indices[3]); // v4
                }
                // Note: This doesn't handle polygons with > 4 vertices robustly,
                // but it will work for standard cubes exported as quads.
            }
            // --- MODIFICATION END ---
        }
        // Ignore other lines like 'vt', 'vn', 'usemtl', 's' etc.
    }

    // Populate m_Vertices using the collected positions, like original code
    // Note: This simple approach copies all positions, which might include unused ones.
    // A more optimized loader would only create vertices referenced by m_Indices.
    for (const auto& pos : positions) {
        m_Vertices.push_back({ pos });
    }

    // Check if we loaded any geometry
     if (m_Vertices.empty() || m_Indices.empty()) {
        std::cerr << "Warning: Mesh data not loaded correctly (Vertices: "
                  << m_Vertices.size() << ", Indices: " << m_Indices.size() << ") from " << path << std::endl;
        // Clean up any potentially half-created buffers if we are returning false
        glDeleteVertexArrays(1, &m_VAO); m_VAO = 0;
        glDeleteBuffers(1, &m_VBO); m_VBO = 0;
        glDeleteBuffers(1, &m_EBO); m_EBO = 0;
        return false;
    }


    // Create VAO, VBO, EBO and upload data - kept from original code
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

    // Setup vertex attributes - kept from original code
    glEnableVertexAttribArray(0); // Position (assuming layout location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Add other attributes here if your Vertex struct and shaders use them (e.g., normals, UVs)

    glBindVertexArray(0); // Unbind VAO
    return true; // Success
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
