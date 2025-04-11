#pragma once
#include <GLFW/glfw3.h>

// Forward declarations
namespace ImGui { class Context; }

class EngineUI {
public:
    EngineUI();
    ~EngineUI();

    void Initialize(GLFWwindow* window);
    void BeginFrame();
    void Render();
    void EndFrame();
    void Shutdown();

private:
    void DrawMainDockspace();
    void DrawViewport();
    void DrawHierarchy();
    void DrawInspector();
    void DrawAssetBrowser();
    void DrawConsole();
    void DrawMenuBar();

    // Dados para os diferentes painéis
    char m_ObjectName[128] = "Player";
    float m_Position[3] = { 0.0f, 0.0f, 0.0f };
    float m_Rotation[3] = { 0.0f, 0.0f, 0.0f };
    float m_Scale[3] = { 1.0f, 1.0f, 1.0f };
    int m_AssetTypeIndex = 0;
};