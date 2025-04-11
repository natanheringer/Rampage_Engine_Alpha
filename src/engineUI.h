#pragma once
#include <GLFW/glfw3.h>
#include "framebuffer.h"
#include "imgui.h"

class EngineUI {
public:
    EngineUI();
    ~EngineUI();

    void Initialize(GLFWwindow* window);
    void BeginFrame();
    void Render();
    void EndFrame();
    void Shutdown();

    void SetFramebuffer(Framebuffer* framebuffer);
    bool IsViewportHovered() const { return m_ViewportHovered; }
    ImVec2 GetViewportSize() const { return m_ViewportSize; }

private:
    void DrawMainDockspace();
    void DrawViewport();
    void DrawHierarchy();
    void DrawInspector();
    void DrawAssetBrowser();
    void DrawConsole();
    void DrawMenuBar();

    // Referência ao framebuffer
    Framebuffer* m_Framebuffer = nullptr;

    // Dados do viewport
    ImVec2 m_ViewportSize = { 1280.0f, 720.0f };
    bool m_ViewportHovered = false;

    // Dados do painel Inspector
    char m_ObjectName[128] = "Player";
    float m_Position[3] = { 0.0f, 0.0f, 0.0f };
    float m_Rotation[3] = { 0.0f, 0.0f, 0.0f };
    float m_Scale[3] = { 1.0f, 1.0f, 1.0f };

    // Dados do painel Assets
    int m_AssetTypeIndex = 0;
};