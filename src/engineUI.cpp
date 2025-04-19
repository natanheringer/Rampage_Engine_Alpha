#include "engineUI.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <string>


void EngineUI::SetFramebuffer(Framebuffer* framebuffer) {
    m_Framebuffer = framebuffer;
}

EngineUI::EngineUI() {
}

EngineUI::~EngineUI() {
}



void EngineUI::Initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();                        // Verifica se a versão do ImGui está correta
    ImGui::CreateContext();                      // Cria o contexto principal do ImGui
    ImGuiIO& io = ImGui::GetIO(); (void)io;      // Acessa configurações de input/output

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Ativa navegação por teclado
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Ativa sistema de docking (arrastar painéis)

    ImGui::StyleColorsDark();                    // Define o estilo dark

    ImGui_ImplGlfw_InitForOpenGL(window, true);  // Inicializa ImGui com GLFW
    ImGui_ImplOpenGL3_Init("#version 130");      // Inicializa renderização com OpenGL
}

void EngineUI::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();      // Nova frame para renderização OpenGL
    ImGui_ImplGlfw_NewFrame();         // Nova frame para eventos do GLFW
    ImGui::NewFrame();                 // Inicia nova frame ImGui
}

void EngineUI::Render() {
    DrawMainDockspace();               // Chama o método que desenha tudo
}

void EngineUI::EndFrame() {
    ImGui::Render();                               // Finaliza o frame
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());  // Desenha na tela
}

void EngineUI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();      // Libera recursos de OpenGL
    ImGui_ImplGlfw_Shutdown();         // Libera recursos do GLFW
    ImGui::DestroyContext();           // Destroi o contexto ImGui
}


void EngineUI::DrawMainDockspace() {
    // Define flags da janela principal
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();       // Pega o viewport da janela principal
    ImGui::SetNextWindowPos(viewport->Pos);                   // Posição
    ImGui::SetNextWindowSize(viewport->Size);                 // Tamanho
    ImGui::SetNextWindowViewport(viewport->ID);               // Associa ao viewport principal
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);  // Estilo: bordas retas
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    // Mais flags
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("DockSpace", nullptr, window_flags);  // Cria a janela invisível
    ImGui::PopStyleVar(2);

    DrawMenuBar();  // Desenha a barra de menus

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");       // ID único para o Dock
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));        // Cria espaço de docking

    // Desenha os painéis
    DrawViewport();
    DrawHierarchy();
    DrawInspector();
    DrawAssetBrowser();
    DrawConsole();

    ImGui::End();
}


//Mostra uma cena com objetos que podem ser expandidos e colapsados.
void EngineUI::DrawViewport() {
    ImGui::Begin("Viewport");

    m_ViewportHovered = ImGui::IsWindowHovered();
    if (m_ViewportHovered) { std::cout << "Viewport Hovered!" << std::endl; }
    m_ViewportSize = ImGui::GetContentRegionAvail();

    if (m_Framebuffer) {
        ImTextureID textureID = (ImTextureID)(uintptr_t)m_Framebuffer->GetTextureID();
        ImGui::Image(textureID, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
    }

    ImGui::End();
}

//painel para editar propriedades de posição, 
//rotação e escala de um objeto selecionado.


void EngineUI::DrawInspector() {
    ImGui::Begin("Inspector");
    ImGui::Text("Propriedades do Objeto Selecionado");
    ImGui::Separator();

    ImGui::Text("Nome: ");
    ImGui::SameLine();
    ImGui::InputText("##nome", m_ObjectName, IM_ARRAYSIZE(m_ObjectName));

    ImGui::Text("Posição:");
    ImGui::DragFloat3("##pos", m_Position, 0.1f);   // XYZ com slider

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

//desenha a hierarquia
void EngineUI::DrawHierarchy() {
    ImGui::Begin("Hierarchy");

    if (ImGui::TreeNode("Scene")) {
        if (ImGui::Selectable("Cubo.obj")) {
            m_SelectedObject = "Cubo.obj";
        }
        ImGui::TreePop();
    }

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