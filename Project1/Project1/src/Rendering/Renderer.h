#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include "../Geometry/Elipse.h"
class Renderer
{
	Elipse elipse;
	Shader shader;
	unsigned int VAO, shaderProgram, texName;
	void CreateTexture();
public:
	Renderer();
	void Init();
	void Render();
};