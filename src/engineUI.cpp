#include "engineUI.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <string>
#include <glm/gtc/type_ptr.hpp> // For ImGui::DragFloat3 with glm vectors
#include <cstring> // For strncpy (Safer C-style string copy)
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include "shader.h"

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
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
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
    
    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
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
    m_ViewportSize = ImGui::GetContentRegionAvail();

    if (m_Framebuffer) {
        ImTextureID textureID = (ImTextureID)(uintptr_t)m_Framebuffer->GetTextureID();
        if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0) { // Prevent ImGui error if size is zero
             ImGui::Image(textureID, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
        }
    }

    // NEW: Handle viewport mouse interaction for object selection and gizmo manipulation
    if (ImGui::IsWindowHovered()) {
        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 windowPos = ImGui::GetWindowPos();
        
        // Convert to viewport coordinates
        ImVec2 viewportMousePos = ImVec2(
            mousePos.x - windowPos.x,
            mousePos.y - windowPos.y
        );
        
        // Debug mouse state
        bool isMouseDown = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
        bool isMouseDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
        bool isMouseHeld = ImGui::IsMouseDown(ImGuiMouseButton_Left);
        
        if (isMouseDown) {
            std::cout << "Mouse clicked at: (" << viewportMousePos.x << ", " << viewportMousePos.y << ")" << std::endl;
        }
        if (isMouseDragging) {
            std::cout << "Mouse dragging detected" << std::endl;
        }
        if (isMouseHeld && !isMouseDown) {
            std::cout << "Mouse held (not dragging)" << std::endl;
        }
        
        // Handle gizmo interaction first
        HandleGizmoInteraction(viewportMousePos, m_ViewportSize, isMouseDown, isMouseDragging);
        
        // Handle object selection (only if not interacting with gizmo)
        if (isMouseDown && m_HoveredAxis == -1 && !m_IsGizmoDragging) {
            HandleViewportClick(viewportMousePos, m_ViewportSize);
        }
    }

    ImGui::End();
}

// NEW: Handle viewport clicking for object selection
void EngineUI::HandleViewportClick(const ImVec2& clickPos, const ImVec2& viewportSize) {
    if (!m_Camera || !m_SceneObjectsPtr) return;

    // Convert screen coordinates to world ray
    glm::vec3 rayOrigin, rayDirection;
    rayOrigin = m_Camera->GetCameraPosition();
    rayDirection = ScreenToWorldRay(clickPos, viewportSize);

    // Pick object
    GameObject* pickedObject = PickObject(rayOrigin, rayDirection);
    if (pickedObject) {
        m_SelectedObjectPtr = pickedObject;
        std::cout << "Selected object: " << pickedObject->name << std::endl;
    } else {
        // Deselect if clicking on empty space
        m_SelectedObjectPtr = nullptr;
        std::cout << "Deselected all objects" << std::endl;
    }
}

// NEW: Convert screen coordinates to world ray
glm::vec3 EngineUI::ScreenToWorldRay(const ImVec2& screenPos, const ImVec2& viewportSize) {
    if (!m_Camera) return glm::vec3(0.0f);

    // Convert screen coordinates to normalized device coordinates (-1 to 1)
    float x = (2.0f * screenPos.x) / viewportSize.x - 1.0f;
    float y = 1.0f - (2.0f * screenPos.y) / viewportSize.y;

    // Create ray in clip space
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

    // Transform to eye space
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), viewportSize.x / viewportSize.y, 0.1f, 100.0f);
    glm::mat4 view = m_Camera->GetViewMatrix();
    
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    // Transform to world space
    glm::vec4 rayWorld = glm::inverse(view) * rayEye;
    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

    return rayDirection;
}

// NEW: Pick object using ray casting
GameObject* EngineUI::PickObject(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) {
    if (!m_SceneObjectsPtr) return nullptr;

    GameObject* closestObject = nullptr;
    float closestDistance = std::numeric_limits<float>::max();

    for (GameObject* obj : *m_SceneObjectsPtr) {
        if (!obj || !obj->mesh) continue;

        // Simple bounding sphere test for now
        glm::vec3 objectPos = obj->transform.position;
        float radius = 1.0f; // Assume unit radius for now

        // Ray-sphere intersection
        glm::vec3 oc = rayOrigin - objectPos;
        float a = glm::dot(rayDirection, rayDirection);
        float b = 2.0f * glm::dot(oc, rayDirection);
        float c = glm::dot(oc, oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant > 0) {
            float t1 = (-b - sqrt(discriminant)) / (2.0f * a);
            float t2 = (-b + sqrt(discriminant)) / (2.0f * a);
            
            if (t1 > 0 && t1 < closestDistance) {
                closestDistance = t1;
                closestObject = obj;
            }
            if (t2 > 0 && t2 < closestDistance) {
                closestDistance = t2;
                closestObject = obj;
            }
        }
    }

    return closestObject;
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

        // NEW: Gizmo mode selector
        ImGui::Text("Gizmo Mode:");
        if (ImGui::RadioButton("Translate", m_GizmoMode == GizmoMode::Translate)) {
            m_GizmoMode = GizmoMode::Translate;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", m_GizmoMode == GizmoMode::Rotate)) {
            m_GizmoMode = GizmoMode::Rotate;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", m_GizmoMode == GizmoMode::Scale)) {
            m_GizmoMode = GizmoMode::Scale;
        }
        
        ImGui::Checkbox("Show Gizmos", &m_ShowGizmos);
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

        // NEW: Draw gizmo info
        ImGui::Separator();
        DrawGizmos();

    } else {
        ImGui::Text("No object selected.");
    }
    ImGui::End();
}

// NEW: Draw gizmos for selected object
void EngineUI::DrawGizmos() {
    if (!m_SelectedObjectPtr || !m_ShowGizmos) return;

    // Get the selected object's world position
    glm::vec3 objectPos = m_SelectedObjectPtr->transform.position;
    
    // Calculate gizmo size based on camera distance
    float gizmoSize = 1.0f; // Fixed size for now
    
    // Draw coordinate axes as simple lines
    // This is a basic implementation - in a real engine you'd use a proper gizmo library
    
    // Note: This would need to be called from the rendering loop, not from ImGui
    // For now, we'll just add a visual indicator in the UI
    ImGui::Text("Gizmo at: (%.2f, %.2f, %.2f)", objectPos.x, objectPos.y, objectPos.z);
    
    // Draw mode indicator
    const char* modeNames[] = {"Translate", "Rotate", "Scale"};
    ImGui::Text("Current Mode: %s", modeNames[static_cast<int>(m_GizmoMode)]);
}

// NEW: Render 3D gizmos in the viewport
void EngineUI::RenderGizmos(Shader* shader, const glm::mat4& view, const glm::mat4& projection) {
    if (!m_SelectedObjectPtr || !m_ShowGizmos) {
        std::cout << "Gizmo rendering skipped - no selected object or gizmos disabled" << std::endl;
        return;
    }

    std::cout << "Rendering gizmos for object: " << m_SelectedObjectPtr->name << std::endl;
    
    // Get the selected object's position
    glm::vec3 objectPos = m_SelectedObjectPtr->transform.position;
    float gizmoSize = 2.0f; // Smaller size to fit inside the cube

    // Use fixed function pipeline for gizmos - this ALWAYS works
    glUseProgram(0); // Disable shader temporarily
    
    // Disable depth testing so gizmos are always visible
    glDisable(GL_DEPTH_TEST);
    
    // Draw coordinate axes using immediate mode for simplicity
    glBegin(GL_LINES);
    
    // X-axis (Red) - Draw from center outward (at object position)
    if (m_HoveredAxis == 0 || m_DraggedAxis == 0) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow when hovered/dragged
    } else {
        glColor3f(1.0f, 0.0f, 0.0f); // Red normally
    }
    glVertex3f(objectPos.x - gizmoSize/2, objectPos.y, objectPos.z);
    glVertex3f(objectPos.x + gizmoSize/2, objectPos.y, objectPos.z);
    
    // Y-axis (Green) - Draw from center outward (at object position)
    if (m_HoveredAxis == 1 || m_DraggedAxis == 1) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow when hovered/dragged
    } else {
        glColor3f(0.0f, 1.0f, 0.0f); // Green normally
    }
    glVertex3f(objectPos.x, objectPos.y - gizmoSize/2, objectPos.z);
    glVertex3f(objectPos.x, objectPos.y + gizmoSize/2, objectPos.z);
    
    // Z-axis (Blue) - Draw from center outward (at object position)
    if (m_HoveredAxis == 2 || m_DraggedAxis == 2) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow when hovered/dragged
    } else {
        glColor3f(0.0f, 0.0f, 1.0f); // Blue normally
    }
    glVertex3f(objectPos.x, objectPos.y, objectPos.z - gizmoSize/2);
    glVertex3f(objectPos.x, objectPos.y, objectPos.z + gizmoSize/2);
    
    glEnd();
    
    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Reset color
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Re-enable the main shader
    shader->Use();
    
    std::cout << "Gizmo rendering completed for: " << m_SelectedObjectPtr->name << std::endl;
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

// NEW: Handle interactive gizmo functionality
void EngineUI::HandleGizmoInteraction(const ImVec2& mousePos, const ImVec2& viewportSize, bool isMouseDown, bool isMouseDragging) {
    if (!m_SelectedObjectPtr || !m_ShowGizmos || m_GizmoMode != GizmoMode::Translate) return;

    // Convert mouse position to world ray
    glm::vec3 rayOrigin = m_Camera->GetCameraPosition();
    glm::vec3 rayDirection = ScreenToWorldRay(mousePos, viewportSize);

    // Check if hovering over gizmo axes
    int hoveredAxis = -1;
    bool isHovered = IsGizmoHovered(rayOrigin, rayDirection, hoveredAxis);
    
    m_HoveredAxis = hoveredAxis;

    // Debug output for hover state
    if (isHovered) {
        std::cout << "Hovering over axis: " << hoveredAxis << std::endl;
    }

    // Handle mouse down
    if (isMouseDown && isHovered) {
        m_IsGizmoDragging = true;
        m_DraggedAxis = hoveredAxis;
        m_GizmoDragStartPos = m_SelectedObjectPtr->transform.position;
        m_LastMousePos = mousePos;
        std::cout << "Started dragging axis: " << hoveredAxis << std::endl;
    }

    // Handle mouse release
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && m_IsGizmoDragging) {
        m_IsGizmoDragging = false;
        m_DraggedAxis = -1;
        std::cout << "Stopped dragging" << std::endl;
    }

    // Handle dragging - check for any mouse movement (including held state)
    if (m_IsGizmoDragging) {
        ImVec2 currentMousePos = mousePos;
        ImVec2 mouseDelta = ImVec2(currentMousePos.x - m_LastMousePos.x, currentMousePos.y - m_LastMousePos.y);
        
        // Check if mouse actually moved (lower threshold for more responsiveness)
        if (abs(mouseDelta.x) > 0.05f || abs(mouseDelta.y) > 0.05f) {
            std::cout << "Mouse delta: (" << mouseDelta.x << ", " << mouseDelta.y << ")" << std::endl;
            std::cout << "Dragged axis: " << m_DraggedAxis << std::endl;
            UpdateGizmoDrag(mouseDelta, viewportSize);
            m_LastMousePos = currentMousePos;
        } else {
            std::cout << "Mouse moved but delta too small: (" << mouseDelta.x << ", " << mouseDelta.y << ")" << std::endl;
        }
    } else if (m_IsGizmoDragging) {
        std::cout << "Gizmo dragging but no movement detected" << std::endl;
    }
}

// NEW: Check if gizmo axis is hovered
bool EngineUI::IsGizmoHovered(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, int& hoveredAxis) {
    if (!m_SelectedObjectPtr) return false;

    glm::vec3 objectPos = m_SelectedObjectPtr->transform.position;
    float gizmoSize = 2.0f; // Match the rendering size
    float hitDistance = 0.5f; // Smaller hit area for precision

    // Check each axis
    for (int axis = 0; axis < 3; axis++) {
        glm::vec3 axisStart = objectPos;
        glm::vec3 axisEnd = objectPos;
        
        // Set axis direction
        if (axis == 0) { // X-axis
            axisStart.x -= gizmoSize/2;
            axisEnd.x += gizmoSize/2;
        } else if (axis == 1) { // Y-axis
            axisStart.y -= gizmoSize/2;
            axisEnd.y += gizmoSize/2;
        } else { // Z-axis
            axisStart.z -= gizmoSize/2;
            axisEnd.z += gizmoSize/2;
        }

        // Line-line distance calculation
        glm::vec3 axisDir = glm::normalize(axisEnd - axisStart);
        glm::vec3 toRay = rayOrigin - axisStart;
        
        float t = glm::dot(toRay, axisDir);
        glm::vec3 closestPoint = axisStart + t * axisDir;
        
        // Clamp to axis bounds
        if (t < 0) closestPoint = axisStart;
        else if (t > gizmoSize) closestPoint = axisEnd;
        
        // Calculate distance from ray to closest point on axis
        glm::vec3 rayPoint = rayOrigin + rayDirection * 10.0f;
        float distance = glm::length(rayPoint - closestPoint);
        
        if (distance < hitDistance) {
            hoveredAxis = axis;
            return true;
        }
    }
    
    hoveredAxis = -1;
    return false;
}

// NEW: Update gizmo drag movement
void EngineUI::UpdateGizmoDrag(const ImVec2& mouseDelta, const ImVec2& viewportSize) {
    if (!m_SelectedObjectPtr || m_DraggedAxis == -1) {
        std::cout << "UpdateGizmoDrag: No selected object or invalid axis" << std::endl;
        return;
    }

    // Use simpler, more direct movement calculation
    float sensitivity = 0.05f; // Reduced for more precise control
    glm::vec3 movement = glm::vec3(0.0f);

    // Calculate movement based on dragged axis
    if (m_DraggedAxis == 0) { // X-axis (Red)
        // Move along world X-axis
        movement = glm::vec3(mouseDelta.x * sensitivity, 0.0f, 0.0f);
        std::cout << "Moving X-axis: delta=" << mouseDelta.x << ", movement=(" << movement.x << "," << movement.y << "," << movement.z << ")" << std::endl;
    } else if (m_DraggedAxis == 1) { // Y-axis (Green)
        // Move along world Y-axis
        movement = glm::vec3(0.0f, -mouseDelta.y * sensitivity, 0.0f);
        std::cout << "Moving Y-axis: delta=" << mouseDelta.y << ", movement=(" << movement.x << "," << movement.y << "," << movement.z << ")" << std::endl;
    } else if (m_DraggedAxis == 2) { // Z-axis (Blue)
        // Move along world Z-axis
        movement = glm::vec3(0.0f, 0.0f, mouseDelta.y * sensitivity);
        std::cout << "Moving Z-axis: delta=" << mouseDelta.y << ", movement=(" << movement.x << "," << movement.y << "," << movement.z << ")" << std::endl;
    }

    // Apply movement to object
    glm::vec3 oldPos = m_SelectedObjectPtr->transform.position;
    m_SelectedObjectPtr->transform.position += movement;
    glm::vec3 newPos = m_SelectedObjectPtr->transform.position;
    
    std::cout << "Object moved from (" << oldPos.x << "," << oldPos.y << "," << oldPos.z 
              << ") to (" << newPos.x << "," << newPos.y << "," << newPos.z << ")" << std::endl;
    
    // Verify the object pointer is valid
    std::cout << "Selected object name: " << m_SelectedObjectPtr->name << std::endl;
}