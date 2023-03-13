#include "Renderer.h"
#include <iostream>
#include <glfw/glfw3.h>
#include "../interfaces/IGui.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SceneObject.h"
#include "../interfaces/IMeshOwner.h"

Renderer::Renderer(Window& window)
    :shader("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag"), window(window)
{
}

void Renderer::Init()
{
    GLenum err = glewInit();
    shader.Init();
    shader.use();
}

void Renderer::Update(Camera& camera)
{
    unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix()* camera.GetViewMatrix()));
}

void Renderer::Render(std::vector<std::shared_ptr<ISceneElement>>& elements, std::vector<std::shared_ptr<IGui>>& guis)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (std::shared_ptr<ISceneElement> el : elements)
    {
        auto mesh = std::dynamic_pointer_cast<IMeshOwner>(el);
        if(mesh)
            mesh->getMesh().Render();
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    for (std::shared_ptr<ISceneElement> el : elements)
    {
        auto gui = std::dynamic_pointer_cast<IGui>(el);
        if (gui)
            gui->RenderGui();
    }

    for (std::shared_ptr<IGui>& gui : guis)
        gui->RenderGui();

    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
