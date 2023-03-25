#include "MainMenu.h"
#include "../Scene/Torus.h"
#include "../Scene/Point.h"
#include "../Scene/Curve.h"

MainMenu::MainMenu(Scene& scene)
    :scene(scene)
{}

void MainMenu::RenderGui()
{

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Add"))
        {
            if (ImGui::MenuItem("Torus", "")) 
            {
                scene.Add(std::make_shared<Torus>());
            }
            if (ImGui::MenuItem("Point", "")) 
            {
                scene.Add(std::make_shared<Point>());
            }
            if (ImGui::MenuItem("Curve", ""))
            {
                scene.Add(std::make_shared<Curve>());
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
