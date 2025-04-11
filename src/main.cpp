#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "engineUI.h"
#include "camera.h"
#include "framebuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW\n";
        return -1;
    }

    // Criar janela
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Rampage Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Falha ao criar janela\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Habilitar VSync

    // Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar GLAD\n";
        return -1;
    }

    // Inicializar EngineUI
    EngineUI engineUI;
    engineUI.Initialize(window);

    // Inicializar framebuffer
    Framebuffer framebuffer(1280, 720);
    engineUI.SetFramebuffer(&framebuffer);

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Obter tamanho atual da janela
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // Verificar tamanho do Viewport do ImGui
        ImVec2 viewportSize = engineUI.GetViewportSize();
        if ((int)viewportSize.x != framebuffer.GetWidth() || (int)viewportSize.y != framebuffer.GetHeight()) {
            framebuffer.Resize((int)viewportSize.x, (int)viewportSize.y);
        }

        // Renderizar cena no framebuffer
        framebuffer.Bind();
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, framebuffer.GetWidth(), framebuffer.GetHeight());
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // =======================
        // RENDERIZAÇÃO OPENGL AQUI
        
        // (Nenhum shader/modelo fornecido)
        // =======================

        framebuffer.Unbind();

        // Renderizar UI
        engineUI.BeginFrame();
        engineUI.Render();
        engineUI.EndFrame();

        // Exibir na janela principal
        glfwSwapBuffers(window);
    }

    // Finalizar
    engineUI.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}