#include "MainMenu.h"
#include "../Scene/Torus.h"
#include "../Scene/Point.h"
#include "../Scene/CurveC0.h"
#include "../Scene/CurveC2.h"
#include "../Scene/InterpolationCurve.h"

MainMenu::MainMenu(Scene& scene, Camera& camera)
    :scene(scene), camera(camera)
{}

bool MainMenu::RenderGui(std::vector<std::shared_ptr<ISceneTracker>>& trackers)
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
            if (ImGui::MenuItem("CurveC0", ""))
            {
                std::vector<std::shared_ptr<Point>> points;
                for(auto& el : scene.objects)
                    if (el.second)
                    {
                        auto point = std::dynamic_pointer_cast<Point>(el.first);
                        if(point)
                            points.push_back(point);
                    }
                auto curve = std::make_shared<CurveC0>(points);
                scene.Add(curve);
                
            }
            if (ImGui::MenuItem("CurveC2", ""))
            {
                std::vector<std::shared_ptr<Point>> points;
                for (auto& el : scene.objects)
                    if (el.second)
                    {
                        auto point = std::dynamic_pointer_cast<Point>(el.first);
                        if (point)
                            points.push_back(point);
                    }
                auto curve = std::make_shared<CurveC2>(points);
                scene.Add(curve);

            }
            if (ImGui::MenuItem("Interpolation Curve", ""))
            {
                std::vector<std::shared_ptr<Point>> points;
                for (auto& el : scene.objects)
                    if (el.second)
                    {
                        auto point = std::dynamic_pointer_cast<Point>(el.first);
                        if (point)
                            points.push_back(point);
                    }
                auto curve = std::make_shared<InterpolationCurve>(points);
                scene.Add(curve);

            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    return false;
}
