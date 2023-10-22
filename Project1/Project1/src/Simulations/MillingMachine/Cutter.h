#pragma once
#include <glm/gtc/type_precision.hpp>
#include "../../interfaces/IRenderable.h"
#include "../../Rendering/Shader.h"

class Cutter : public IRenderable {
	Shader shader;

	unsigned int VBO, EBO, VAO;
	int indexCount;
	glm::fmat4x4 translation;
	
	void createMesh(int divisions);
	std::tuple<float, float> sampleBase(int divisions, int index);
public:
	Cutter();
	void setPosition(glm::fvec3 pos);
	// Inherited via IRenderable
	virtual void Render(bool selected, VariableManager& vm) override;
};