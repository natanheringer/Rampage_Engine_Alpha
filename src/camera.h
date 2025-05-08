#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(float radius = 5.0f, float yaw = 0.0f, float pitch = 20.0f);

    // metodos de controle da camera
    // esses metodos sao chamados pelo GLFW quando o usuario interage com a camera
    
    void ProcessMouseMovement(float deltaX, float deltaY);
    void ProcessMousePan(float deltaX, float deltaY);
    void ProcessMouseScroll(float deltaScroll);
    
    glm::mat4 GetViewMatrix() const;
    glm::vec3 GetCameraPosition() const;

private:
    float m_Yaw, m_Pitch;
    float m_Distance;
    glm::vec3 m_FocusPoint;
};