#include "Renderer.h"
#include <iostream>
#include <glfw/glfw3.h>

Renderer::Renderer(Window& window, Elipse& elipse)
    :shader("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag"), window(window)
{
}

void Renderer::Init()
{
    GLenum err = glewInit();
    shader.Init();
    shader.use();
    glUniform1i(glGetUniformLocation(shader.ID, "texture"), 0);
}

void Renderer::Update()
{

    if (window.curentMouseVectorX != 0 || window.curentMouseVectorY != 0)
    {
        isMoving = true;
    }
    else
        isMoving = false;
}

void Renderer::Render(std::vector<Mesh>& meshes, std::vector<std::shared_ptr<IGui>>& guis)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (Mesh& mesh : meshes)
        mesh.Render();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    for (std::shared_ptr<IGui>& gui : guis)
        gui->Render();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
