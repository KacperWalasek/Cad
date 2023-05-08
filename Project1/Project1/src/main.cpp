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
#include "EventHandlers/ObjectFactory.h"
#include "Rendering/VariableManager.h"

int main()
{

    Window window;
    Renderer renderer(window);

    std::shared_ptr<Camera> camera = std::make_shared<Camera>(window,1,0.1,1000);
    camera->transform.location.z = -100;

    window.Init();
    renderer.Init();
    std::shared_ptr<Scene> scene = std::make_shared<Scene>(
        std::vector<std::pair<std::shared_ptr<ISceneElement>,bool>>(),
        *camera);

    std::vector<std::shared_ptr<IGui>> guis = {
        camera,
        scene, 
        std::make_shared<MainMenu>(*scene, *camera),
        scene->cursor
    };

    auto movement = std::make_shared<CameraMovement>(*camera);
    window.mouseCallbacks = { 
        movement, 
        scene->cursor, 
        std::make_shared<ClickSelection>(*scene,*camera)};

    auto objMovement = std::make_shared<SelectedMovement>(*scene, *camera);
    window.keyCallbacks = { 
        objMovement,
        std::make_shared<ObjectFactory>(*scene)
    };
    auto a = GL_MAX_TESS_GEN_LEVEL;
    
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