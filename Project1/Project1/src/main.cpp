#include "Math/Matrix4x4.h"
#include <iostream>
#include "Rendering/Window.h"
#include "Rendering/Renderer.h"
#include "UI/ElipseGui.h"

int main()
{
    Window window;
    Elipse elipse;
    Renderer renderer(window,elipse);
    window.Init();
    renderer.Init();
    ElipseGui elipseGui(elipse);

    while (window.isOpen())
    {
        window.Update();
        renderer.Update(elipse);
        renderer.Render(elipse, elipseGui);
        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }

	return 0;
}