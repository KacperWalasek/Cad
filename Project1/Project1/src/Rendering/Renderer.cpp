#include "Renderer.h"
#include <iostream>
#include <glfw/glfw3.h>
#include "../interfaces/IGui.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SceneObject.h"
#include "../interfaces/IRenderable.h"
#include "../Rotator.h"
#include "../interfaces/ITransformable.h"

Renderer::Renderer(Window& window)
    :shader("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag"), window(window)
{
}

void Renderer::Init()
{
    GLenum err = glewInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    shader.Init();
    shader.use();
    glPointSize(5);
    
    variableManager.AddVariable("transform", glm::identity<glm::fmat4x4>());
    variableManager.AddVariable("color", glm::fvec4());
    variableManager.AddVariable("t0", 0.0f);
    variableManager.AddVariable("t1", 0.0f);
}

void Renderer::Update(Camera& camera)
{
}

void Renderer::Render(Camera& camera, Scene& scene, std::vector<std::shared_ptr<IGui>>& guis)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& el : scene.objects)
    {
        shader.use();
        if (el.second)
        {
            glLineWidth(2);
            if (el.first == scene.lastSelected)
                variableManager.SetVariable("color", glm::fvec4(1.0f, 0.8f, 0.0f, 1.0f ));
            else
                variableManager.SetVariable("color", glm::fvec4(1.0f, 0.5f, 0.0f, 1.0f));
        }
        else
            variableManager.SetVariable("color", glm::fvec4(1.0f, 1.0f, 1.0f, 1.0f));

        auto renderable = std::dynamic_pointer_cast<IRenderable>(el.first);
        if (renderable)
        {
            glm::fmat4x4 matrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();

            auto objTransformable = std::dynamic_pointer_cast<ITransformable>(el.first);
            if (objTransformable) 
                matrix = matrix * objTransformable->getTransform().GetMatrix();
            variableManager.SetVariable("transform", matrix);
            variableManager.Apply(shader.ID);
            renderable->Render(el.second, variableManager);
        }
        glLineWidth(1);
    }
    shader.use();
    
    scene.cursor->transform.scale = camera.transform.scale;
    glm::fmat4x4 matrix = camera.GetProjectionMatrix() * camera.GetViewMatrix() * scene.cursor->transform.GetMatrix();

    variableManager.SetVariable("transform", matrix);
    variableManager.SetVariable("color", glm::fvec4(0.5f, 0.5f, 1.0f, 1.0f));
    variableManager.Apply(shader.ID);
    scene.cursor->Render(false, variableManager);
    
    if(std::find_if(scene.objects.begin(), scene.objects.end(), [](auto& o) { return o.second; })!= scene.objects.end())
    {
        scene.center.transform.scale = camera.transform.scale;
        glm::fmat4x4 centerMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix() * scene.center.transform.GetMatrix();

        variableManager.SetVariable("transform", centerMatrix);
        variableManager.SetVariable("color", glm::fvec4(1.0f, 0.0f, 0.0f, 1.0f));
        variableManager.Apply(shader.ID);
        scene.center.Render(false, variableManager);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    auto selectedGui = std::dynamic_pointer_cast<IGui>(scene.lastSelected);
    if (selectedGui)
        selectedGui->RenderGui();
        
    for (std::shared_ptr<IGui>& gui : guis)
        gui->RenderGui();

    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
