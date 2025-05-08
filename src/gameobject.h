#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // for glm::translate, rotate, scale
#include "mesh.h" 

// transform class that points to a mesh and has position, rotation and scale
// this class is used to transform the mesh in the world space
struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale    = glm::vec3(1.0f);


        glm::mat4 GetModelMatrix() const {
            glm::mat4 model = glm::mat4(1.0f); // Identity matrix
            model = glm::translate(model, position); // Apply translation
            model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X axis
            model = glm ::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y axis
            model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z axis

                model = glm::scale(model, scale); // Apply scaling
                return model; 
        }
};

class GameObject {
    public: 
    std::string name;
    Transform transform;

    Mesh* mesh = nullptr; // Pointer to the mesh object uses 
    // unsigned int textureID = 0; // add later if objects have unique textures 

    GameObject(const std::string& name = "GameObject", Mesh* mesh = nullptr)
        : name(name), mesh(mesh) {}


    // More components will be added later, components such as:
    // Rigidbody, Collider, Light, Camera, etc. 



};