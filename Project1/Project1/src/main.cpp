#include <iostream>
#include "Rendering/Window.h"
#include "Rendering/Renderer.h"
#include "UI/IGui.h"

int main()
{
    Window window;
    Renderer renderer(window);
    window.Init();
    renderer.Init();
    std::vector<std::shared_ptr<IGui>> guis;

    Mesh mesh;
    mesh.vertices = {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
    };
    mesh.indices = {
        0, 1, 3,
        1, 2, 3
    };
    mesh.Init();
    std::vector<Mesh> meshes{mesh};

    while (window.isOpen())
    {
        window.Update();
        renderer.Update();
        renderer.Render(meshes, guis);
        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }

	return 0;
}