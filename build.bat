@echo off
echo Building Rampage Engine...

g++ -std=c++17 ^
-Iinclude ^
-Itinyobjloader-release ^
-Iimgui ^
-Iimgui/backends ^
-Iglad/include ^
-Iglm ^
-Llib ^
-lglfw3 ^
-lopengl32 ^
-lgdi32 ^
-luser32 ^
-lkernel32 ^
src/main.cpp ^
src/camera.cpp ^
src/shader.cpp ^
src/mesh.cpp ^
src/framebuffer.cpp ^
src/engineUI.cpp ^
src/texture.cpp ^
src/material.cpp ^
src/texture_generator.cpp ^
src/glad.c ^
tinyobjloader-release/tiny_obj_loader.cc ^
imgui/imgui.cpp ^
imgui/imgui_demo.cpp ^
imgui/imgui_draw.cpp ^
imgui/imgui_tables.cpp ^
imgui/imgui_widgets.cpp ^
imgui/backends/imgui_impl_glfw.cpp ^
imgui/backends/imgui_impl_opengl3.cpp ^
-o build/Rampage_Engine_Alpha.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo Running engine...
    build/Rampage_Engine_Alpha.exe
) else (
    echo Build failed!
    pause
)