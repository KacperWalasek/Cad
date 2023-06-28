#pragma once 
#include "../interfaces/IRenderable.h"
#include "../Rendering/Shader.h"

class SceneVector : public IRenderable
{
	unsigned int VAO, EBO, VBO;
	Shader shader;
	glm::fvec4 color;
public:
	SceneVector(glm::fvec3 position, glm::fvec3 end, glm::fvec4 color);

	// Inherited via IRenderable
	virtual void Render(bool selected, VariableManager& vm) override;

};