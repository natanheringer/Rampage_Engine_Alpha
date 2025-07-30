#pragma once
#include <GLFW/glfw3.h>
#include "framebuffer.h"
#include "imgui.h"
#include <string>
#include <vector> // For std::vector
#include "gameobject.h" // include new GameObject 
#include <glm/glm.hpp>

class EngineUI {
public:
    EngineUI();
    ~EngineUI();

    void Initialize(GLFWwindow* window);
    void BeginFrame();
    void Render(); // handle the scene objects
    void EndFrame();
    void Shutdown();

    void SetFramebuffer(Framebuffer* framebuffer);
    bool IsViewportHovered() const { return m_ViewportHovered; }
    ImVec2 GetViewportSize() const { return m_ViewportSize; }

    // new additions 

    void SetSceneObjects(std::vector<GameObject*>* objects); // Pass a pointer
    // to the scene's object list 
    GameObject* GetSelectedObject() const { return m_SelectedObjectPtr;}

    // NEW: Object selection by clicking
    void SetCamera(class Camera* camera) { m_Camera = camera; }
    void HandleViewportClick(const ImVec2& clickPos, const ImVec2& viewportSize);

    // NEW: 3D Gizmo rendering
    void RenderGizmos(class Shader* shader, const glm::mat4& view, const glm::mat4& projection);
    
    // NEW: Interactive gizmo functionality
    void HandleGizmoInteraction(const ImVec2& mousePos, const ImVec2& viewportSize, bool isMouseDown, bool isMouseDragging);
    bool IsGizmoHovered(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, int& hoveredAxis);
    void UpdateGizmoDrag(const ImVec2& mouseDelta, const ImVec2& viewportSize);

private:
    void DrawMainDockspace();
    void DrawViewport();
    void DrawHierarchy();
    void DrawInspector();
    void DrawAssetBrowser();
    void DrawConsole();
    void DrawMenuBar();
    void DrawGizmos(); // NEW: Draw gizmos for selected object

    // NEW: Ray casting for object selection
    GameObject* PickObject(const glm::vec3& rayOrigin, const glm::vec3& rayDirection);
    glm::vec3 ScreenToWorldRay(const ImVec2& screenPos, const ImVec2& viewportSize);

    // framebuffer reference 
    Framebuffer* m_Framebuffer = nullptr;

    // viewport data
    ImVec2 m_ViewportSize = { 1280.0f, 720.0f };
    bool m_ViewportHovered = false;

   /* // Removed - Placeholders inspactor data panel 
    char m_ObjectName[128] = "Player";
    float* GetPosition() { return m_Position; }
    float* GetRotation() { return m_Rotation; }
    float* GetScale()    { return m_Scale;    }
    float m_Position[3] = { 0.0f, 0.0f, 0.0f };
    float m_Rotation[3] = { 0.0f, 0.0f, 0.0f };
    float m_Scale[3] = { 1.0f, 1.0f, 1.0f };*/

    std::vector<GameObject*>* m_SceneObjectsPtr = nullptr; // Ponter to the actual list of objects in the scene
    GameObject* m_SelectedObjectPtr = nullptr; // Pointer to the selected object

    // NEW: Camera reference for ray casting
    class Camera* m_Camera = nullptr;

    // NEW: Gizmo state
    enum class GizmoMode {
        Translate,
        Rotate,
        Scale
    };
    bool m_ShowGizmos = true;
    GizmoMode m_GizmoMode = GizmoMode::Translate;
    
    // NEW: Interactive gizmo state
    bool m_IsGizmoDragging = false;
    int m_HoveredAxis = -1;
    int m_DraggedAxis = -1;
    ImVec2 m_LastMousePos = {0, 0};
    glm::vec3 m_GizmoDragStartPos = glm::vec3(0.0f);
    
    // NEW: Gizmo shader for proper rendering
    Shader* m_GizmoShader = nullptr;

    // Asset Browser data (Will be expanded later)
    int m_AssetTypeIndex = 0;
    //std::string m_SelectedObject; removed, it was replaced by m_SelectedObjectPtr

};