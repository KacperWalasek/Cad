#include "MainMenu.h"
#include "../Scene/Torus.h"
#include "../Scene/Point.h"
#include "../Scene/Curve.h"

MainMenu::MainMenu(Scene& scene, Camera& camera)
    :scene(scene), camera(camera)
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
                std::vector<std::shared_ptr<Point>> points;
                for(auto& el : scene.objects)
                    if (el.second)
                    {
                        auto point = std::dynamic_pointer_cast<Point>(el.first);
                        if(point)
                            points.push_back(point);
                    }
                auto curve = std::make_shared<Curve>(camera, points);
                scene.Add(curve);
                
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
