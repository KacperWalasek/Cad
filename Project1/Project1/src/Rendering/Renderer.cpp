#include "Renderer.h"
#include <iostream>
#include <glfw/glfw3.h>

Renderer::Renderer(Window& window, Elipse& elipse)
    :shader("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag"), window(window), 
    adaptiveRendering([&elipse](int x, int y) { return elipse.CalculatePixelColor(x, y); },1600,1600.f/900.f, 160, 2)
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
    {
        if (!isMoving)
            adaptiveRendering.hardReset = true;
        isMoving = true;
        adaptiveRendering.Reset();
    }
    else
        isMoving = false;
    adaptiveRendering.Checkout();
}

void Renderer::Render(Elipse& elipse, std::vector<std::shared_ptr<IGui>>& guis)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    adaptiveRendering.Render();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    for (std::shared_ptr<IGui>& gui : guis)
        gui->Render();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
