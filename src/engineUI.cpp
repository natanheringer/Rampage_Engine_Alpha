#include "engineUI.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <string>
#include <glm/gtc/type_ptr.hpp> // For ImGui::DragFloat3 with glm vectors
#include <cstring> // For strncpy (Safer C-style string copy)


void EngineUI::SetFramebuffer(Framebuffer* framebuffer) {
    m_Framebuffer = framebuffer;
}

// --- NEW: To link scene objects from main.cpp ---
void EngineUI::SetSceneObjects(std::vector<GameObject*>* objects) {
    m_SceneObjectsPtr = objects;
}

EngineUI::EngineUI() {
    // Constructor might initialize m_SelectedObjectPtr to nullptr if needed,
    // but it's default initialized as a member.
}

EngineUI::~EngineUI() {
}

void EngineUI::Initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void EngineUI::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EngineUI::Render() {
    DrawMainDockspace(); // This will call all other Draw... panel methods
}

void EngineUI::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EngineUI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void EngineUI::DrawMainDockspace() {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // --- CORRECTED: Only one ImGui::Begin/End for DockSpace ---
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(2); // Pop the two style vars pushed earlier

    DrawMenuBar();

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

    // Draw individual panels
    DrawViewport();
    DrawHierarchy();
    DrawInspector();
    DrawAssetBrowser();
    DrawConsole();

    ImGui::End(); // End DockSpace
}

void EngineUI::DrawViewport() {
    ImGui::Begin("Viewport");
    m_ViewportHovered = ImGui::IsWindowHovered();
    // Removed the debug print: if (m_ViewportHovered) { std::cout << "Viewport Hovered!" << std::endl; }
    m_ViewportSize = ImGui::GetContentRegionAvail();

    if (m_Framebuffer) {
        ImTextureID textureID = (ImTextureID)(uintptr_t)m_Framebuffer->GetTextureID();
        if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0) { // Prevent ImGui error if size is zero
             ImGui::Image(textureID, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
        }
    }
    ImGui::End();
}

void EngineUI::DrawInspector() {
    ImGui::Begin("Inspector");
    if (m_SelectedObjectPtr) {
        GameObject* obj = m_SelectedObjectPtr;

        char nameBuffer[128];
        // Use strncpy for safer string copy to avoid buffer overflow
        strncpy(nameBuffer, obj->name.c_str(), sizeof(nameBuffer) - 1);
        nameBuffer[sizeof(nameBuffer) - 1] = '\0'; // Ensure null termination

        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
            obj->name = nameBuffer;
        }
        ImGui::Separator();

        ImGui::Text("Transform");
        if (ImGui::DragFloat3("Position", glm::value_ptr(obj->transform.position), 0.1f)) {
            // Value changed directly
        }
        if (ImGui::DragFloat3("Rotation", glm::value_ptr(obj->transform.rotation), 1.0f)) {
            // Value changed directly
        }
        if (ImGui::DragFloat3("Scale", glm::value_ptr(obj->transform.scale), 0.05f)) {
            if (obj->transform.scale.x <= 0.0001f) obj->transform.scale.x = 0.0001f;
            if (obj->transform.scale.y <= 0.0001f) obj->transform.scale.y = 0.0001f;
            if (obj->transform.scale.z <= 0.0001f) obj->transform.scale.z = 0.0001f;
        }

        if (obj->mesh) {
            ImGui::Separator();
            // In a real scenario, you might get the mesh's asset path or a user-friendly name
            ImGui::Text("Mesh: %s", "Assigned Mesh"); // Generic placeholder
        }

    } else {
        ImGui::Text("No object selected.");
    }
    ImGui::End();
}

void EngineUI::DrawAssetBrowser() {
    ImGui::Begin("Assets");
    if (ImGui::Button("Load Cube Mesh (Test)")) {
        if (m_SelectedObjectPtr) {
            std::cout << "Asset Browser: 'Load Cube Mesh' clicked for " << m_SelectedObjectPtr->name << std::endl;
            // This functionality needs a MeshManager to be truly useful.
            // For now, it's a placeholder action.
        }
    }
    ImGui::Separator();
    static const char* items[] = { "Modelos", "Texturas", "Sons", "Scripts", "Materiais", "Prefabs" };
    ImGui::Combo("Tipo", &m_AssetTypeIndex, items, IM_ARRAYSIZE(items));
    ImGui::Separator();
    for (int i = 0; i < 10; i++) {
        ImGui::Button(("Asset " + std::to_string(i)).c_str(), ImVec2(100, 100));
        if ((i + 1) % 4 != 0) ImGui::SameLine();
    }
    ImGui::End();
}

void EngineUI::DrawConsole() {
    ImGui::Begin("Console");
    ImGui::Text("Inicializando Rampage Engine...");
    ImGui::Text("Sistema de renderização inicializado.");
    // ImGui::Text("Sistema de física carregado."); // Commented out if not yet implemented
    ImGui::Text("Aguardando comandos...");
    ImGui::End();
}

void EngineUI::DrawHierarchy() {
    ImGui::Begin("Hierarchy");
    if (m_SceneObjectsPtr) {
        // Create a temporary copy for safe iteration if objects might be deleted during UI interaction
        // For now, direct iteration is fine as we are only selecting.
        for (size_t i = 0; i < m_SceneObjectsPtr->size(); ++i) {
            GameObject* obj = (*m_SceneObjectsPtr)[i];
            if (obj) {
                ImGui::PushID(obj); // Use pointer as unique ID
                bool is_selected = (m_SelectedObjectPtr == obj);
                if (ImGui::Selectable(obj->name.c_str(), is_selected)) {
                    m_SelectedObjectPtr = obj;
                }
                if (is_selected && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive()) {
                     // ImGui::SetItemDefaultFocus(); // Can be a bit aggressive with focus
                }
                ImGui::PopID();
            }
        }
    }
    ImGui::End();
}

void EngineUI::DrawMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Arquivo")) {
            if (ImGui::MenuItem("Novo", "Ctrl+N")) { /* ... */ }
            if (ImGui::MenuItem("Abrir", "Ctrl+O")) { /* ... */ }
            if (ImGui::MenuItem("Salvar", "Ctrl+S")) { /* ... */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Sair", "Alt+F4")) {
                // Need a way to signal main loop to close, e.g., by setting glfwWindowShouldClose
                // For now, this doesn't do anything.
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Editar")) {
            if (ImGui::MenuItem("Desfazer", "Ctrl+Z")) { /* ... */ }
            if (ImGui::MenuItem("Refazer", "Ctrl+Y")) { /* ... */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Configurações")) { /* ... */ }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}