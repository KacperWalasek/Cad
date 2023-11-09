#pragma once
#include "../interfaces/IRenderable.h"
#include "../Rendering/VariableManager.h"

class BrokenLine : public IRenderable
{
	std::vector<glm::fvec3> points;
	void updateMesh(bool cullLong);

	unsigned int VAO, VBO, EBO;
	int indsSize;
	glm::fvec2 cull(glm::fvec2 x, glm::fvec2 dir);
public:
	void setPoints(std::vector<glm::fvec3> points, bool cullLong = false);
	std::vector<glm::fvec3> getPoints() const;
	BrokenLine();
	

	virtual void Render(bool selected, VariableManager& vm) override;
};