
===============================================[Rampage Engine Code Functions Explained]===============================================


Overall Structure:

This is a basic 3D engine structure using C++, OpenGL (via GLAD and GLFW), GLM for math, and ImGui for the user interface. The goal seems to be creating a simple editor environment where a 3D scene is rendered into a texture, which is then displayed within an ImGui panel (the "Viewport").
File Breakdown:

main.cpp

Purpose: 

    The main entry point and central coordinator of the application.

Responsibilities:
    
    Initializes GLFW (windowing and input library).
    Creates the main application window.
    Initializes GLAD (loads OpenGL function pointers).
    Initializes the EngineUI system (passing the GLFW window).
    Creates a Framebuffer object to render the 3D scene into.
    Connects the Framebuffer to the EngineUI so the UI can display its contents.
    Creates a Camera, a Shader (loading vertex/fragment files), and a Mesh (loading a cube model).
    Contains the main application loop:
    Processes events (like keyboard/mouse input).
    Gets the current size of the ImGui viewport panel from EngineUI.
    Resizes the Framebuffer if the ImGui viewport size has changed.
    Renders the 3D Scene:
    Binds the Framebuffer (directs rendering to it).
    Clears the framebuffer and sets up the OpenGL viewport.
    Calculates Model, View (from Camera), and Projection matrices using GLM.
    Uses the Shader, sets the matrix uniforms.
    Draws the Mesh (the cube).
    Unbinds the Framebuffer (stops rendering to it).
    Renders the UI: Calls EngineUI methods to draw the ImGui interface.
    Swaps the main window buffers (displays the final image, which includes the UI).
    Cleans up resources (EngineUI, GLFW) upon closing.
    Dependencies: Uses almost all other components: EngineUI, Camera, Framebuffer, Shader, Mesh, plus external libraries (GLFW, GLAD, GLM, ImGui).

shader.h / shader.cpp

Purpose: 
    Manages OpenGL Shader Programs.

Responsibilities:

    Loads shader source code from vertex and fragment shader files (.vert, .frag).
    Compiles the individual vertex and fragment shaders.
    Links them together into a single shader program.
    Provides error checking for compilation and linking.
    Provides a Use() method to activate the shader program for rendering.
    Provides methods like SetMat4() to send data (like transformation matrices) to the shader uniforms.
    Dependencies: Uses GLAD (for OpenGL functions), GLM (for matrix types), <string>, <fstream>, <sstream>, <iostream>.
    camera.h / camera.cpp
    Purpose: Represents the virtual camera in the 3D scene.
    Responsibilities:
    Stores camera orientation (yaw, pitch) and position (defined by distance from a focus point).
    Processes mouse input to update orientation (orbit), position (pan), and distance (zoom).
    Calculates and provides the View Matrix, which transforms world coordinates into camera coordinates for rendering.
    Provides the camera's world position.
    Dependencies: Uses GLM (for vector/matrix math).

engineui.h / engineui.cpp

Purpose: 
    Manages the Dear ImGui user interface.

Responsibilities:

    Initializes and shuts down the ImGui context and its backends (GLFW, OpenGL3).
    Sets up ImGui features like docking.
    Provides BeginFrame() and EndFrame() wrappers for ImGui frame management.
    Defines the structure of the UI using Draw... methods for different panels:
    DrawMainDockspace: Sets up the main docking area.
    DrawViewport: Creates the panel where the 3D scene is displayed. Crucially, it gets the texture ID from the Framebuffer (via SetFramebuffer and GetTextureID) and uses ImGui::Image to draw it. It also reports its size and hover state.
    DrawHierarchy, DrawInspector, DrawAssetBrowser, DrawConsole: Creates placeholder panels common in game editors.
    DrawMenuBar: Creates the top menu bar.
    Holds a pointer to the Framebuffer to access the rendered scene texture.
    Dependencies: Uses ImGui, GLFW, Framebuffer.

framebuffer.h / framebuffer.cpp

Purpose: 
    Encapsulates an OpenGL Framebuffer Object (FBO). This allows rendering the scene to a texture instead of directly to the window.

Responsibilities:

    Creates and manages the underlying OpenGL FBO, a color texture attachment, and a depth/stencil renderbuffer attachment.
    Provides Bind() and Unbind() methods to switch rendering between this framebuffer and the default window framebuffer.
    Provides access to the generated color texture's OpenGL ID (GetTextureID()), which is needed by EngineUI to display it.
    Provides a Resize() method to change the size of the texture and renderbuffer attachments (used when the ImGui viewport panel resizes).
    Handles cleanup of OpenGL resources in its destructor.
    Dependencies: Uses GLAD (for OpenGL functions), <iostream>.

mesh.h / mesh.cpp

Purpose: 
 
    Represents 3D model geometry data and its OpenGL buffers.

Responsibilities:

    Defines a Vertex structure (currently just position).
    Loads vertex positions and face indices from a simple .obj file format (ignoring normals, texture coordinates for now).
    Creates and manages OpenGL Vertex Array Object (VAO), Vertex Buffer Object (VBO), and Element Buffer Object (EBO).
    Uploads vertex and index data to the GPU via the VBO and EBO.
    Configures vertex attribute pointers within the VAO (telling OpenGL how to interpret the vertex data in the VBO).
    Provides a Draw() method that binds the VAO and issues an OpenGL draw call (glDrawElements).
    Handles cleanup of OpenGL resources in its destructor.
    Dependencies: Uses GLAD (for OpenGL functions), GLM (for vectors), <string>, <vector>, <fstream>, <sstream>, <iostream>.

glad.c (Not pasted, but assumed)

Purpose: 
    
    OpenGL Loading Library.

Responsibilities: 

    Contains the implementation code generated by the GLAD service. It finds and loads the addresses of OpenGL functions at runtime, making them available to use after an OpenGL context is created.
    Dependencies: System OpenGL libraries.

Relationships and Flow:

    main.cpp starts everything, initializes GLFW, GLAD, and ImGui via EngineUI.
    main.cpp creates engine objects: Camera, Shader, Mesh, Framebuffer.
    main.cpp tells EngineUI which Framebuffer to use (SetFramebuffer).

In the main loop:
    a. main.cpp checks if the EngineUI viewport panel size changed. If so, it calls Framebuffer::Resize.
    b. main.cpp binds the Framebuffer (Framebuffer::Bind).
    c. main.cpp gets the view matrix from Camera::GetViewMatrix.
    d. main.cpp uses the Shader (Shader::Use, Shader::SetMat4) and tells the Mesh to draw itself (Mesh::Draw). This rendering goes into the Framebuffer's texture.
    e. main.cpp unbinds the Framebuffer (Framebuffer::Unbind).
    f. main.cpp tells EngineUI to render (EngineUI::BeginFrame, Render, EndFrame).
    g. Inside EngineUI::Render (specifically DrawViewport), EngineUI gets the texture ID from the Framebuffer (Framebuffer::GetTextureID) and uses ImGui::Image to draw that texture inside the "Viewport" window panel.
    h. main.cpp swaps the window's buffers, showing the complete ImGui interface (which contains the rendered scene within one of its panels).

    This setup allows you to have a 3D scene rendered independently and then integrated seamlessly into a complex editor UI.