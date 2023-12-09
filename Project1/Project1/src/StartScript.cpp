#include "StartScript.h"
#include "UI/FileLoader.h"
#include "Geometry/Torus.h"
#include "Geometry/Curves/CurveC0.h"
#include "Geometry/Curves/CurveC2.h"
#include "Geometry/Surfaces/SurfaceC2.h"
#include "Geometry/Surfaces/SurfaceC0.h"
#include "Milling/MillingPathCreator.h"
#include "Simulations/MillingMachine/MillingMachineSimulation.h"

StartScript::StartScript(Scene& scene, Simulator& simulator, ErrorDisplayer& errorDisplayer)
    : scene(scene), simulator(simulator), errorDisplayer(errorDisplayer)
{
} 

void StartScript::Run()
{
    if (dissabled)
        return;
    const std::string modelPath = "E:\\Workspace\\Git\\Cad\\Cad\\Project1\\Project1\\klodkaNaprawionaOst.json";
    const std::string pathPath = "E:\\Workspace\\Git\\Cad\\Cad\\Project1\\Project1\\Paths2\\script.k16";

    if (!modelPath.empty())
    {
        nlohmann::json json = FileLoader::load(modelPath);

        std::map<int, std::shared_ptr<Point>> pointIndexMap;

        for (nlohmann::json pJson : json["points"])
        {
            auto p = std::make_shared<Point>(pJson);
            pointIndexMap.insert({ pJson["id"],p });
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
    
    auto mp = MillingPathCreator::CreateRoughtingPath(scene);
    FileLoader::savePath(pathPath, mp);

    
    auto simulation = std::make_shared<MillingMachineSimulation>();
    simulator.simulations.push_back(simulation);
    scene.Add(simulation);
    errorDisplayer.AddHandler(simulation->GetErrorHandler());
    simulation->LoadPaths({ pathPath });

    //simulation->AddPath(mp);
}
