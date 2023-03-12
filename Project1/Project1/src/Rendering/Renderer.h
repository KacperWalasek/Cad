#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include "Window.h"
#include "Mesh.h"
#include "../UI/IGui.h"

class Renderer
{
	Window& window;
	Shader shader;
	bool isMoving = false;
public:
	Renderer(Window& window);
	void Init();
	void Update();
	void Render(std::vector<Mesh>& meshes, std::vector<std::shared_ptr<IGui>>& guis);
};