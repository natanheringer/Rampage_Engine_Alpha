#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "engineUI.h"
#include "camera.h"
#include "framebuffer.h"
#include "shader.h"
#include "mesh.h"


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


    Camera camera;
    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    Mesh cube;
    cube.LoadFromOBJ("assets/Cube.obj");

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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // View/Proj matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)framebuffer.GetWidth() / framebuffer.GetHeight(), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        shader.Use();
        shader.SetMat4("model", model);


        // Render
        shader.Use();
        shader.SetMat4("model", model);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);
        cube.Draw();
         // Cria o shader
        Shader shader("shaders/basic.vert", "shaders/basic.frag");

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