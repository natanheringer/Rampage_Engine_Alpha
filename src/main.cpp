#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>       // For std::vector
#include <string>       // For std::string

#include "engineUI.h"
#include "camera.h"
#include "framebuffer.h"
#include "shader.h"
#include "mesh.h"
#include "gameobject.h" // <<< --- ADD THIS INCLUDE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h> // Make sure imgui is included

int main() {
    // --- Initialization (GLFW, Window, GLAD) ---
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW\n";
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Rampage Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Falha ao criar janela\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable VSync
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar GLAD\n";
        glfwDestroyWindow(window); // Clean up window if GLAD fails
        glfwTerminate();
        return -1;
    }

    // --- Initialization (Engine Systems) ---
    EngineUI engineUI;
    engineUI.Initialize(window);

    Framebuffer framebuffer(1280, 720);
    engineUI.SetFramebuffer(&framebuffer); // Link framebuffer to UI

    Camera camera(10.0f); // Initial camera distance
    Shader shader("shaders/basic.vert", "shaders/basic.frag");

    // --- MESH LOADING (Load meshes ONCE that can be shared) ---
    Mesh cubeMesh; // Create a Mesh object
    if (!cubeMesh.LoadFromOBJ("assets/Cube.obj")) { // Load into the Mesh object
         std::cerr << "Falha ao carregar assets/Cube.obj. Saindo." << std::endl;
         engineUI.Shutdown();
         glfwDestroyWindow(window);
         glfwTerminate();
         return -1;
    }
    // You can load more distinct meshes here if needed:
    // Mesh sphereMesh;
    // if (!sphereMesh.LoadFromOBJ("assets/Sphere.obj")) { /* ... */ }


    // --- SCENE SETUP (GameObjects) ---
    std::vector<GameObject*> sceneObjects; // Our list of game objects in the scene

    GameObject* cubeObject1 = new GameObject("MyFirstCube", &cubeMesh); // Pass pointer to loaded cubeMesh
    cubeObject1->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    sceneObjects.push_back(cubeObject1);

    GameObject* cubeObject2 = new GameObject("AnotherCube", &cubeMesh); // Can reuse the same mesh
    cubeObject2->transform.position = glm::vec3(2.5f, 0.5f, -1.0f); // Give it a different position
    cubeObject2->transform.rotation = glm::vec3(0.0f, 45.0f, 0.0f); // And rotation
    cubeObject2->transform.scale    = glm::vec3(0.75f);             // And scale
    sceneObjects.push_back(cubeObject2);

    // GameObject* sphereObject = new GameObject("MySphere", &sphereMesh); // If you had a sphere mesh
    // sphereObject->transform.position = glm::vec3(-2.0f, 0.0f, 0.0f);
    // sceneObjects.push_back(sphereObject);

    engineUI.SetSceneObjects(&sceneObjects); // <<< --- PASS THE SCENE TO THE UI ---


    // --- Variables for Camera Control ---
    static bool isDraggingOrbit = false;
    static bool isDraggingPan = false;
    static ImVec2 lastMousePos = {0, 0};

    // --- Main Loop ---
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // Process window events
        engineUI.BeginFrame(); // Start the ImGui frame

        // --- Camera Control Logic (Using ImGui state) ---
        // (This block remains the same as your working version)
        ImGuiIO& io = ImGui::GetIO();
        bool isViewportHovered = engineUI.IsViewportHovered();
        if (isViewportHovered) {
            if (io.MouseWheel != 0.0f) { camera.ProcessMouseScroll(io.MouseWheel); }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) { isDraggingOrbit = true; lastMousePos = io.MousePos; }
            if (isDraggingOrbit && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                ImVec2 currentMousePos = io.MousePos;
                float deltaX = currentMousePos.x - lastMousePos.x;
                float deltaY = currentMousePos.y - lastMousePos.y;
                lastMousePos = currentMousePos;
                if (deltaX != 0 || deltaY != 0) { camera.ProcessMouseMovement(deltaX, deltaY); }
            } else { isDraggingOrbit = false; }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) { isDraggingPan = true; lastMousePos = io.MousePos; }
            if (isDraggingPan && ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
                ImVec2 currentMousePos = io.MousePos;
                float deltaX = currentMousePos.x - lastMousePos.x;
                float deltaY = currentMousePos.y - lastMousePos.y;
                lastMousePos = currentMousePos;
                if (deltaX != 0 || deltaY != 0) { camera.ProcessMousePan(deltaX, deltaY); }
            } else { isDraggingPan = false; }
        } else { isDraggingOrbit = false; isDraggingPan = false; }
        // --- End Camera Control Logic ---


        // --- Framebuffer Resizing ---
        ImVec2 viewportSize = engineUI.GetViewportSize();
        if (viewportSize.x > 0 && viewportSize.y > 0 &&
            ((int)viewportSize.x != framebuffer.GetWidth() || (int)viewportSize.y != framebuffer.GetHeight())) {
            framebuffer.Resize((unsigned int)viewportSize.x, (unsigned int)viewportSize.y);
        }

        // --- Render Scene to Framebuffer ---
        framebuffer.Bind();
        glEnable(GL_DEPTH_TEST);
        int fbWidth = framebuffer.GetWidth();
        int fbHeight = framebuffer.GetHeight();

        if (fbWidth > 0 && fbHeight > 0) {
            glViewport(0, 0, fbWidth, fbHeight);
            // Using a more neutral clear color for better visibility of objects
            glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)fbWidth / (float)fbHeight, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();

            shader.Use();
            shader.SetMat4("view", view); // Set view and projection once per frame
            shader.SetMat4("projection", projection);

            // --- RENDER ALL GAMEOBJECTS ---
            for (GameObject* obj : sceneObjects) {
                if (obj && obj->mesh) { // Check if obj and its mesh are valid
                    glm::mat4 model = obj->transform.GetModelMatrix(); // Get model matrix from GameObject's transform
                    shader.SetMat4("model", model); // Set model matrix for this specific object
                    obj->mesh->Draw(); // Draw the GameObject's mesh
                }
            }
        }
        framebuffer.Unbind();

        // --- Render ImGui UI ---
        engineUI.Render();
        engineUI.EndFrame();

        // --- Swap Buffers ---
        glfwSwapBuffers(window);
    }

    // --- Cleanup ---
    engineUI.Shutdown();

    // Clean up dynamically allocated GameObjects
    for (GameObject* obj : sceneObjects) {
        delete obj;
    }
    sceneObjects.clear();
    // Meshes (like cubeMesh), Shader, Framebuffer will be cleaned up by their destructors
    // as they are stack-allocated in main.

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}