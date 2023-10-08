#include "MainMenu.h"
#include "../Geometry/Torus.h"
#include "../Geometry/Point.h"
#include "../Geometry/Curves/CurveC0.h"
#include "../Geometry/Curves/CurveC2.h"
#include "../Geometry/Curves/InterpolationCurve.h"
#include "../Geometry/Surfaces/SurfaceCreator.h"
#include "../Geometry/Surfaces/SurfaceC0.h"
#include "../Simulations/MillingMachine/MillingMachineSimulation.h"
#include "FileLoader.h"
#include "nlohmann/json.hpp"
#include "tinyfiledialogs/tinyfiledialogs.h"

using nlohmann::json;

MainMenu::MainMenu(Scene& scene, Camera& camera, Simulator& simulator)
    :scene(scene), camera(camera), simulator(simulator)
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
            if (ImGui::MenuItem("Milling Simulation",""))
            {
                auto simulation = std::make_shared<MillingMachineSimulation>();
                simulator.simulations.push_back(simulation);
                scene.Add(simulation);
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Load",""))
        {
            std::string filename = FileLoader::selectFile();
            if (!filename.empty())
            {
                nlohmann::json json = FileLoader::load(filename);

                std::map<int, std::shared_ptr<Point>> pointIndexMap;

                for (nlohmann::json pJson : json["points"])
                {
                    auto p = std::make_shared<Point>(pJson);
                    pointIndexMap.insert({ pJson["id"],p});
                    scene.Add(p, false);
                }
                for (nlohmann::json gJson : json["geometry"])
                {
                    std::string type = gJson["objectType"];
                    if (type == "torus")
                        scene.Add(std::make_shared<Torus>(gJson), false);
                    if (type == "bezierC0")
                        scene.Add(std::make_shared<CurveC0>(gJson, pointIndexMap), false);
                    if (type == "bezierC2")
                        scene.Add(std::make_shared<CurveC2>(gJson, pointIndexMap), false);
                    if (type == "interpolatedC2")
                    {
                        scene.Add(std::make_shared<InterpolationCurve>(gJson, pointIndexMap), false);
                    }
                    if (type == "bezierSurfaceC0")
                    {
                        auto surface = std::make_shared<SurfaceC0>(gJson, pointIndexMap);
                        for (auto p : surface->points)
                            p->po.push_back(surface);
                        scene.Add(surface);
                    }
                    if (type == "bezierSurfaceC2")
                    {
                        auto surface = std::make_shared<SurfaceC2>(gJson, pointIndexMap);
                        for (auto p : surface->points)
                            p->po.push_back(surface);
                        scene.Add(surface);
                        
                    }
                }


            }
        }
        if (ImGui::MenuItem("Save", ""))
        {
            Indexer indexer;
            std::vector<json> points;
            std::vector<json> geometry;
            std::map<int, int> pointIndexMap;

            for (auto& el : scene.objects)
            {
                auto point = std::dynamic_pointer_cast<Point>(el.first);
                if (!point)
                    continue;
                points.push_back(point->Serialize(scene, indexer, pointIndexMap));
            }

            for (auto& el : scene.objects)
            {
                auto point = std::dynamic_pointer_cast<Point>(el.first);
                auto serializable = std::dynamic_pointer_cast<ISerializable>(el.first);
                if (!serializable || point)
                    continue;
                json geoJson = serializable->Serialize(scene, indexer, pointIndexMap);
                if (geoJson.is_array())
                    for (auto& j : geoJson)
                        geometry.push_back(j);
                else
                    geometry.push_back(geoJson);

                
            }


            json sceneJson = {
                {"points", points},
                {"geometry", geometry}
            };
            
            std::string saveDest = FileLoader::selectSaveDest();
            if (!saveDest.empty())
                FileLoader::save(saveDest, sceneJson);
        }
        
        ImGui::EndMainMenuBar();
    }
    return false;
}
