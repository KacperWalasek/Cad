#include <iostream>
#include "Rendering/Window.h"
#include "Rendering/Renderer.h"
#include "EventHandlers/CameraMovement.h"
#include "Rendering/Sampler.h"
#include "Rendering/SceneObject.h"
#include "Scene/Torus.h"
#include "Scene/Scene.h"
#include "EventHandlers/SelectedMovement.h"
#include <memory>
#include "Scene/Point.h"
#include "UI/MainMenu.h"
#include "EventHandlers/ClickSelection.h"
#include "Scene/Curve.h"

int main()
{
    Window window;
    Renderer renderer(window);

    std::shared_ptr<Camera> camera = std::make_shared<Camera>(window,1,0.1,1000);
    camera->transform.location.z = -100;

    window.Init();
    renderer.Init();
    auto p1 = std::make_shared<Point>();
    auto p2 = std::make_shared<Point>();
    auto p3 = std::make_shared<Point>();
    auto p4 = std::make_shared<Point>();
    auto p5 = std::make_shared<Point>();
    auto p6 = std::make_shared<Point>();
    auto p7 = std::make_shared<Point>();
    std::shared_ptr<Scene> scene = std::make_shared<Scene>(
        std::vector<std::pair<std::shared_ptr<ISceneElement>,bool>>( {
            {p1,false},
            {p2,false},
            {p3,false},
            {p4,false},
            {p5,false},
            {p6,false},
            {p7,false},
            std::make_pair(std::make_shared<Curve>(std::vector<std::shared_ptr<Point>>{
                p1,p2,p3,p4,p5,p6,p7
            }),false)
            //std::make_shared<Point>()
            
        }),
        *camera);

    std::vector<std::shared_ptr<IGui>> guis = {
        camera,
        scene, 
        std::make_shared<MainMenu>(*scene),
        scene->cursor
    };

    auto movement = std::make_shared<CameraMovement>(*camera);
    window.mouseCallbacks = { 
        movement, 
        scene->cursor, 
        std::make_shared<ClickSelection>(*scene,*camera)};
    auto objMovement = std::make_shared<SelectedMovement>(*scene, *camera);
    window.keyCallbacks = { objMovement };

    while (window.isOpen())
    {
        window.Update();
        renderer.Update(*camera);
        movement->Update(window.window);
        objMovement->Update(window.window);
        scene->cursor->Update(window.window);
        renderer.Render(*camera, *scene, guis);
        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }

	return 0;
}