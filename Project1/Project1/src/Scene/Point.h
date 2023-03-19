#pragma once
#include "ISceneElement.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/IGui.h"
#include <GL/glew.h>
class Point : public ISceneElement, public IRenderable, public IGui
{
	unsigned int VAO, VBO, EBO;
	glm::fvec3 zero;
	int zero2; 

	Transform transform;
	std::string name;
public:
	Point();
	virtual std::string getName() const override;
	virtual Transform& getTransform() override;

	virtual void Render() override;

	virtual void RenderGui() override;
};