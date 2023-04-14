#pragma once
#include "ISceneElement.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/ITransformable.h"
#include "../interfaces/IGui.h"
#include "../interfaces/IClickable.h"
#include <GL/glew.h>
#include "Scene.h"
#include "../Rendering/Camera.h"

class Point : public ISceneElement, public ITransformable, public IRenderable, public IGui, public IClickable
{
	unsigned int VAO, VBO, EBO;
	glm::fvec3 zero;
	int zero2; 

	Transform transform;
	std::string name;
public:
	Point();
	Point(glm::fvec4 position);
	virtual std::string getName() const override;
	virtual Transform& getTransform() override;
	const Transform& getTransform() const;

	virtual void Render(bool selected) override;

	virtual void RenderGui() override;

	virtual std::tuple<bool, float> InClickRange(Camera& camera, float x, float y) const override;
	virtual bool Click(Scene& scene, Camera& camera, float x, float y) override;
};