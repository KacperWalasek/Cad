#include <iostream>
#include "Rendering/Window.h"
#include "Rendering/Renderer.h"
#include "EventHandlers/CameraMovement.h"
#include "Rendering/Sampler.h"
#include "Rendering/SceneObject.h"
#include "Scene/Torus.h"

int main()
{
    Window window;
    Renderer renderer(window);

    Camera camera(window,3.1,0.1,1000000);
    camera.transform.location.z = -100;

    window.Init();
    renderer.Init();

    std::vector<std::shared_ptr<IGui>> guis;
    std::vector<std::shared_ptr<ISceneElement>> objects = {
        std::make_shared<Torus>()
    };

    auto movement = std::make_shared<CameraMovement>(camera);
    window.mouseCallbacks = { movement };

    while (window.isOpen())
    {
        window.Update();
        renderer.Update(camera);
        movement->Update(window.window);
        renderer.Render(objects, guis);
        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }

	return 0;
}