#include <GLFW/glfw3.h>
#include <iostream>
#include "engineUI.h"
#include <C:/Users/Natan/Documents/GitHub/Rampage_Engine_Alpha/include/GLFW/glm-master/glm-master/glm/glm.hpp>
#include <C:/Users/Natan/Documents/GitHub/Rampage_Engine_Alpha/include/GLFW/glm-master/glm-master/glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    glfwSwapInterval(1); // Enable vsync
    
    // Criar e inicializar a UI da engine
    EngineUI engineUI;
    engineUI.Initialize(window);

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    
        // Preparar frame ImGui
        engineUI.BeginFrame();
    
        // Limpar tela com cor de fundo antes de desenhar ImGui
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    
        // Renderizar UI da engine
        engineUI.Render();
    
        // Finalizar frame (renderizar ImGui por cima da tela limpa)
        engineUI.EndFrame();
    
        // Trocar buffers
        glfwSwapBuffers(window);
    }
    glm::vec3 pos(1.0f, 2.0f, 3.0f);
    std::cout << "X: " << pos.x << "\n";
   
    // Limpar
    engineUI.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}