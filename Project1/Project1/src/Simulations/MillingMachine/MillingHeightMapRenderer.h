#pragma once
#include "../../interfaces/IRenderable.h"
#include "../../Rendering/Shader.h"
class MillingHeightMapRenderer : public IRenderable {
	unsigned int VAO, VBO, EBO;
	std::vector<float> vert;
	std::vector<int> inds;
	Shader shader;

	void flush();
public:
	MillingHeightMapRenderer();
	void Clear();
	void AddSegment(glm::fvec3 p1, glm::fvec3 p2, float r);
	virtual void Render(bool selected, VariableManager& vm) override;
};