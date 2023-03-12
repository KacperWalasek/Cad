#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include "Window.h"
#include "../ElipsoidFiles/Elipse.h"
#include "../ElipsoidFiles/ElipseGui.h"
#include "../ElipsoidFiles/AdaptiveRendering.h"
#include "Mesh.h"

class Renderer
{
	Window& window;
	Shader shader;
	bool isMoving = false;
public:
	Renderer(Window& window, Elipse& elipse);
	void Init();
	void Update();
	void Render(std::vector<Mesh>& meshes, std::vector<std::shared_ptr<IGui>>& guis);
};