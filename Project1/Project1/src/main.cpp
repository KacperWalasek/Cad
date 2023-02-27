#include "Math/Matrix4x4.h"
#include <iostream>
#include "Rendering/Window.h"
#include "Rendering/Renderer.h"
int main()
{
    Window window;
    Renderer renderer;
    window.Init();
    renderer.Init();

    while (window.isOpen())
    {
        renderer.Render();
        window.Update();
    }

	return 0;
}