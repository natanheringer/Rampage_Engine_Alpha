#include "Camera.h"

Camera::Camera(float radius, float yaw, float pitch)
    : m_Distance(radius), m_Yaw(yaw), m_Pitch(pitch), m_FocusPoint(0.0f, 0.0f, 0.0f) {}

void Camera::ProcessMouseMovement(float deltaX, float deltaY) {
    m_Yaw += deltaX * 0.25f;
    m_Pitch -= deltaY * 0.25f;
    m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);
}

void Camera::ProcessMousePan(float deltaX, float deltaY) {
    glm::vec3 right = glm::vec3(
        glm::cos(glm::radians(m_Yaw)), 0.0f, -glm::sin(glm::radians(m_Yaw))
    );
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 pan = (-right * deltaX + up * deltaY) * 0.01f;

    m_FocusPoint += pan;
}

void Camera::ProcessMouseScroll(float deltaScroll) {
    m_Distance -= deltaScroll * 0.5f;
    if (m_Distance < 1.0f) m_Distance = 1.0f;
}

glm::mat4 Camera::GetViewMatrix() const {
    float yawRad = glm::radians(m_Yaw);
    float pitchRad = glm::radians(m_Pitch);

    glm::vec3 direction = {
        glm::cos(pitchRad) * glm::sin(yawRad),
        glm::sin(pitchRad),
        glm::cos(pitchRad) * glm::cos(yawRad)
    };

    glm::vec3 position = m_FocusPoint - direction * m_Distance;
    return glm::lookAt(position, m_FocusPoint, glm::vec3(0, 1, 0));
}

glm::vec3 Camera::GetCameraPosition() const {
    float yawRad = glm::radians(m_Yaw);
    float pitchRad = glm::radians(m_Pitch);
    glm::vec3 direction = {
        glm::cos(pitchRad) * glm::sin(yawRad),
        glm::sin(pitchRad),
        glm::cos(pitchRad) * glm::cos(yawRad)
    };
    return m_FocusPoint - direction * m_Distance;
}