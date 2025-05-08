#pragma once
#include <GLFW/glfw3.h>
#include "framebuffer.h"
#include "imgui.h"
#include <string>
#include <vector> // For std::vector
#include "gameobject.h" // include new GameObject 



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


private:
    void DrawMainDockspace();
    void DrawViewport();
    void DrawHierarchy();
    void DrawInspector();
    void DrawAssetBrowser();
    void DrawConsole();
    void DrawMenuBar();

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



    // Asset Browser data (Will be expanded later)
    int m_AssetTypeIndex = 0;
    //std::string m_SelectedObject; removed, it was replaced by m_SelectedObjectPtr

};