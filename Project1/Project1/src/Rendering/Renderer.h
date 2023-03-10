#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include "Window.h"
#include "../ElipsoidFiles/Elipse.h"
#include "../ElipsoidFiles/ElipseGui.h"
#include "../ElipsoidFiles/AdaptiveRendering.h"

class Renderer
{
	Window& window;
	Shader shader;
	bool isMoving = false;
public:
	AdaptiveRendering adaptiveRendering;
	Renderer(Window& window, Elipse& elipse);
	void Init();
	void Update(Elipse& elipse);
	void Render(Elipse& elipse, std::vector<std::shared_ptr<IGui>>& guis);
};