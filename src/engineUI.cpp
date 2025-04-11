#include "engineUI.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <iostream>
#include <string>

EngineUI::EngineUI() {
}

EngineUI::~EngineUI() {
}

void EngineUI::Initialize(GLFWwindow* window) {
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Habilita navegação por teclado
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Habilita docking
    
    // Setup ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void EngineUI::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EngineUI::Render() {
    DrawMainDockspace();
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
    // Configurações para janela de docking
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    // Criar janela principal
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(2);
    
    // Menu bar
    DrawMenuBar();
    
    // Configurar DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    
    // Desenhar os diferentes painéis
    DrawViewport();
    DrawHierarchy();
    DrawInspector();
    DrawAssetBrowser();
    DrawConsole();
    
    ImGui::End(); // DockSpace
}

void EngineUI::DrawViewport() {
    ImGui::Begin("Viewport");
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::Text("Tamanho do Viewport: %.0fx%.0f", viewportSize.x, viewportSize.y);
    ImGui::End();
}

void EngineUI::DrawHierarchy() {
    ImGui::Begin("Hierarquia");
    if (ImGui::TreeNode("Cena")) {
        if (ImGui::TreeNode("Camera")) {
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Player")) {
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Ambiente")) {
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    ImGui::End();
}

void EngineUI::DrawInspector() {
    ImGui::Begin("Inspector");
    ImGui::Text("Propriedades do Objeto Selecionado");
    ImGui::Separator();
    ImGui::Text("Nome: ");
    ImGui::SameLine();
    ImGui::InputText("##nome", m_ObjectName, IM_ARRAYSIZE(m_ObjectName));
    
    ImGui::Text("Posição:");
    ImGui::DragFloat3("##pos", m_Position, 0.1f);
    
    ImGui::Text("Rotação:");
    ImGui::DragFloat3("##rot", m_Rotation, 0.1f);
    
    ImGui::Text("Escala:");
    ImGui::DragFloat3("##scale", m_Scale, 0.1f);
    ImGui::End();
}

void EngineUI::DrawAssetBrowser() {
    ImGui::Begin("Assets");
    static const char* items[] = { "Modelos", "Texturas", "Sons", "Scripts", "Materiais", "Prefabs" };
    ImGui::Combo("Tipo", &m_AssetTypeIndex, items, IM_ARRAYSIZE(items));
    
    ImGui::Separator();
    
    // Simulação de uma lista de assets
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
    ImGui::Text("Sistema de física carregado.");
    ImGui::Text("Aguardando comandos...");
    ImGui::End();
}

void EngineUI::DrawMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Arquivo")) {
            if (ImGui::MenuItem("Novo", "Ctrl+N")) {}
            if (ImGui::MenuItem("Abrir", "Ctrl+O")) {}
            if (ImGui::MenuItem("Salvar", "Ctrl+S")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Sair", "Alt+F4")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Editar")) {
            if (ImGui::MenuItem("Desfazer", "Ctrl+Z")) {}
            if (ImGui::MenuItem("Refazer", "Ctrl+Y")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Configurações")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}