#include "Math/Matrix4x4.h"
#include <iostream>
#include "Rendering/Window.h"
#include "Rendering/Renderer.h"

int main()
{
    Window window;
    Renderer renderer(window);
    window.Init();
    renderer.Init();

    while (window.isOpen())
    {
        window.Update();
        renderer.Update(window.lastTouchTime);
        renderer.Render();
        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }

	return 0;
}