#include "Renderer.h"
#include <iostream>
#include <glfw/glfw3.h>
#include "../interfaces/IGui.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SceneObject.h"
#include "../interfaces/IRenderable.h"

Renderer::Renderer(Window& window)
    :shader("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag"), window(window)
{
}

void Renderer::Init()
{
    GLenum err = glewInit();
    shader.Init();
    shader.use();
    glPointSize(5);
}

void Renderer::Update(Camera& camera)
{
    /*unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix()* camera.GetViewMatrix()));*/
}

void Renderer::Render(Camera& camera, Scene& scene, std::vector<std::shared_ptr<IGui>>& guis)
{
    unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (std::shared_ptr<ISceneElement> el : scene.objects)
    {
        auto renderable = std::dynamic_pointer_cast<IRenderable>(el);
        if (renderable)
        {
            glm::fmat4x4 matrix = camera.GetProjectionMatrix() * camera.GetViewMatrix() * el->getTransform().GetMatrix();
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));
            renderable->Render();
        }
    } 
    
    glm::fmat4x4 matrix = camera.GetProjectionMatrix() * camera.GetViewMatrix() * scene.cursor->transform.GetMatrix();
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));
    scene.cursor->Render();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    auto selectedGui = std::dynamic_pointer_cast<IGui>(scene.selected);
    if (selectedGui)
        selectedGui->RenderGui();

    for (std::shared_ptr<IGui>& gui : guis)
        gui->RenderGui();

    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
