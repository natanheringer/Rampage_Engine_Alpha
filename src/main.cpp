#include <GLFW/glfw3.h>
#include <iostream>
#include "engineUI.h"

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
        
        // Preparar frame
        engineUI.BeginFrame();
        
        // Renderizar UI da engine
        engineUI.Render();
        
        // Finalizar frame
        engineUI.EndFrame();
        
        // Renderização OpenGL
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Trocar buffers
        glfwSwapBuffers(window);
    }

    // Limpar
    engineUI.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}