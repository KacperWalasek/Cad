#include "Renderer.h"
#include <iostream>
#include <glfw/glfw3.h>

Renderer::Renderer(Window& window, Elipse& elipse)
    :shader("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag"), window(window), 
    adaptiveRendering([&elipse](int x, int y) { return elipse.CalculatePixelColor(x, y); },1600,1600.f/900.f, 16, 2)
{
}

void Renderer::Init()
{
    GLenum err = glewInit();
    shader.Init();
    adaptiveRendering.Init();
    shader.use();
    glUniform1i(glGetUniformLocation(shader.ID, "texture"), 0);
}

void Renderer::Update(Elipse& elipse)
{
    elipse.Update(window);
    if (window.curentMouseVectorX != 0 || window.curentMouseVectorY != 0)
        adaptiveRendering.Reset();
    adaptiveRendering.Checkout();
}

void Renderer::Render(Elipse& elipse, ElipseGui& gui)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    adaptiveRendering.Render();
    //elipse.Render();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    gui.Render();
    
    ImGui::ShowDemoWindow();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
