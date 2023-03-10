#include "Math/Matrix4x4.h"
#include <iostream>
#include "Rendering/Window.h"
#include "Rendering/Renderer.h"
#include "ElipsoidFiles/ElipseGui.h"
#include "ElipsoidFiles/AdaptiveGui.h"

int main()
{
    Window window;
    Elipse elipse;
    Renderer renderer(window,elipse);
    window.Init();
    renderer.Init();
    std::vector<std::shared_ptr<IGui>> guis{ std::make_shared<ElipseGui>(elipse, renderer.adaptiveRendering), std::make_shared<AdaptiveGui>(renderer.adaptiveRendering) };

    while (window.isOpen())
    {
        window.Update();
        renderer.Update(elipse);
        renderer.Render(elipse, guis);
        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }

	return 0;
}