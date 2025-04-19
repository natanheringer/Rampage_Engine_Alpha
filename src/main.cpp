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

    Shader shader("shaders/basic.vert", "shaders/basic.frag"); // Create shader ONCE here
    Mesh cube;
    if (!cube.LoadFromOBJ("assets/Cube.obj")) { // Load mesh and check for errors
         std::cerr << "Falha ao carregar assets/Cube.obj. Saindo." << std::endl;
         engineUI.Shutdown();
         glfwDestroyWindow(window);
         glfwTerminate();
         return -1;
    }


    // --- Variables for Camera Control ---
    static bool isDraggingOrbit = false;
    static bool isDraggingPan = false;
    static ImVec2 lastMousePos = {0, 0};

    // --- Main Loop ---
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // Process window events

        // Start the ImGui frame - MUST be done before querying ImGui state
        engineUI.BeginFrame();

// --- Camera Control Logic (Using ImGui state) ---
ImGuiIO& io = ImGui::GetIO();
bool isViewportHovered = engineUI.IsViewportHovered();

// --- Process inputs ONLY if viewport is hovered ---
// We remove the !io.WantCaptureMouse check here, as it was preventing
// drag initiation. This is often necessary for viewport controls.
if (isViewportHovered)
{
    // 1. Zooming (Scroll Wheel)
    if (io.MouseWheel != 0.0f) {
        // std::cout << "  Scroll Detected: " << io.MouseWheel << std::endl; // DEBUG
        camera.ProcessMouseScroll(io.MouseWheel);
    }

    // 2. Orbiting (Right Mouse Button Drag)
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
         // std::cout << "  Orbit Start (RMB Clicked)" << std::endl; // DEBUG
         isDraggingOrbit = true;
         lastMousePos = io.MousePos; // Store initial drag position
    }
    // Continue dragging if the button is held down (regardless of WantCaptureMouse now)
    if (isDraggingOrbit && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        ImVec2 currentMousePos = io.MousePos;
        float deltaX = currentMousePos.x - lastMousePos.x;
        float deltaY = currentMousePos.y - lastMousePos.y;
        lastMousePos = currentMousePos;

        if (deltaX != 0 || deltaY != 0) {
             // std::cout << "  Orbit Dragging: dx=" << deltaX << ", dy=" << deltaY << std::endl; // DEBUG
             camera.ProcessMouseMovement(deltaX, deltaY);
        }
    } else {
        isDraggingOrbit = false; // Stop dragging if button released
    }


    // 3. Panning (Middle Mouse Button Drag)
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
         // std::cout << "  Pan Start (MMB Clicked)" << std::endl; // DEBUG
         isDraggingPan = true;
         lastMousePos = io.MousePos; // Store initial drag position
    }
     // Continue dragging if the button is held down (regardless of WantCaptureMouse now)
     if (isDraggingPan && ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        ImVec2 currentMousePos = io.MousePos;
        float deltaX = currentMousePos.x - lastMousePos.x;
        float deltaY = currentMousePos.y - lastMousePos.y;
        lastMousePos = currentMousePos;

        if (deltaX != 0 || deltaY != 0) {
             // std::cout << "  Pan Dragging: dx=" << deltaX << ", dy=" << deltaY << std::endl; // DEBUG
            camera.ProcessMousePan(deltaX, deltaY);
        }
    } else {
        isDraggingPan = false; // Stop dragging if button released
    }
}
else // If viewport NOT hovered, ensure dragging stops
{
     isDraggingOrbit = false;
     isDraggingPan = false;
}
// --- End Camera Control Logic ---


        // --- Framebuffer Resizing ---
        // Check if Viewport size from ImGui has changed
        ImVec2 viewportSize = engineUI.GetViewportSize();
        // Ensure viewport size is valid before resizing
        if (viewportSize.x > 0 && viewportSize.y > 0 &&
            ((int)viewportSize.x != framebuffer.GetWidth() || (int)viewportSize.y != framebuffer.GetHeight()))
        {
            framebuffer.Resize((unsigned int)viewportSize.x, (unsigned int)viewportSize.y);
        }

        // --- Render Scene to Framebuffer ---
        framebuffer.Bind(); // Render to our framebuffer
        glEnable(GL_DEPTH_TEST);

        // Get framebuffer dimensions (might have changed)
        int fbWidth = framebuffer.GetWidth();
        int fbHeight = framebuffer.GetHeight();

        // Only render if framebuffer size is valid
        if (fbWidth > 0 && fbHeight > 0)
        {
            glViewport(0, 0, fbWidth, fbHeight); // Set viewport to framebuffer size
            glClearColor(1.0f, 0.1f, 2.0f, 1.0f); // Your magenta-ish clear color
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth

            // Calculate matrices - GET VIEW MATRIX *AFTER* CAMERA UPDATE
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)fbWidth / (float)fbHeight, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix(); // Use updated camera matrix
            glm::mat4 model = glm::mat4(1.0f); // Place object at origin (camera focuses on origin)
            // If you want the cube offset from the focus point, apply translation:
            // model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

            // Setup shader and draw the cube
            shader.Use(); // Use the shader program
            shader.SetMat4("model", model);
            shader.SetMat4("view", view);
            shader.SetMat4("projection", projection);
            // Optional: glDisable(GL_CULL_FACE); if needed
            cube.Draw(); // Draw the mesh
        }

        framebuffer.Unbind(); // Stop rendering to our framebuffer

        // --- Render ImGui UI ---
        // engineUI.BeginFrame(); // Already called above!
        engineUI.Render(); // Draw all ImGui windows (including viewport displaying the framebuffer texture)
        engineUI.EndFrame(); // Renders ImGui draw data to the default framebuffer (the window)

        // --- Swap Buffers ---
        glfwSwapBuffers(window); // Display the final rendered window content
    }

    // --- Cleanup ---
    engineUI.Shutdown();
    // Note: Mesh/Shader/Framebuffer will clean up via their destructors when they go out of scope
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}