#include "MainMenu.h"
#include "../Geometry/Torus.h"
#include "../Geometry/Point.h"
#include "../Geometry/Curves/CurveC0.h"
#include "../Geometry/Curves/CurveC2.h"
#include "../Geometry/Curves/InterpolationCurve.h"
#include "../Geometry/Surfaces/SurfaceCreator.h"
#include "FileLoader.h"
#include <nlohmann/json-schema.hpp>

using nlohmann::json;
using nlohmann::json_schema::json_validator;

MainMenu::MainMenu(Scene& scene, Camera& camera)
    :scene(scene), camera(camera)
{}

bool MainMenu::RenderGui()
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
            if (ImGui::MenuItem("Surface", ""))
            {
                scene.Add(std::make_shared<SurfaceCreator>(scene));
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Load",""))
        {
            json ex1 = json::parse(R"(
              {
                "pi": 3.141,
                "happy": true
              }
            )");

            std::string filename = FileLoader::selectFile();
            if (!filename.empty())
            {

            }
        }
        
        ImGui::EndMainMenuBar();
    }
    return false;
}
