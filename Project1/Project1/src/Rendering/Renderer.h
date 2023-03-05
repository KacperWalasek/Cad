#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include "Window.h"
#include "../Geometry/Elipse.h"
#include <future>

class Renderer
{
	Window& window;
	Elipse elipse;
	Shader shader;
	std::future<RGB*> imageFuture;

	unsigned int VAO, shaderProgram, texName;
	void CreateTexture(float lastToutchTime);
public:
	Renderer(Window& window);
	void Init();
	void Update(float lastToutchTime);
	void Render();
};