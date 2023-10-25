#pragma once
#include "../../interfaces/IRenderable.h"
#include "../../Rendering/Shader.h"
class MillingHeightMapRenderer : public IRenderable {
	unsigned int VAO, VBO, EBO;
	std::vector<float> vert;
	std::vector<int> inds;
	std::vector<float> temporaryVert;
	std::vector<int> temporaryInds;
	Shader shader;
	bool flat;
	float radius;

	void flush();
	void addSegment(std::vector<float>& vertices, std::vector<int>& indices, glm::fvec3 p1, glm::fvec3 p2, float r);
	void clearTemporary();
public:
	MillingHeightMapRenderer();
	void Clear();
	void AddSegment(glm::fvec3 p1, glm::fvec3 p2, float r);
	void AddTemporarySegment(glm::fvec3 p1, glm::fvec3 p2, float r);
	virtual void Render(bool selected, VariableManager& vm) override;
	void setRadius(float radius);
	void setFlat(bool flat);
};