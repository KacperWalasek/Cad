#pragma once
#include "../interfaces/ISceneElement.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/ITransformable.h"
#include "../interfaces/IGui.h"
#include "../interfaces/IClickable.h"
#include "../interfaces/ISelfControl.h"
#include "../interfaces/IPointOwner.h"
#include "../Scene/Scene.h"
#include "../Rendering/Camera.h"
#include "../Indexer.h"
#include <GL/glew.h>

class Point : public ISceneElement, public ITransformable, public IRenderable, public IGui, public IClickable, public ISelfControl
{
	static Indexer indexer;

	unsigned int VAO, VBO, EBO;
	glm::fvec3 zero;
	int zero2; 

	Transform transform;
	std::string name;

public:
	std::weak_ptr<IPointOwner> po;

	Point(std::string name);
	Point();
	Point(glm::fvec4 position);
	Point(glm::fvec4 position, std::string name);
	virtual std::string getName() const override;
	virtual Transform& getTransform() override;
	const Transform& getTransform() const;

	virtual void Render(bool selected, VariableManager& vm) override;

	virtual bool RenderGui() override;

	virtual std::tuple<bool, float> InClickRange(Camera& camera, float x, float y) const override;
	virtual bool Click(Scene& scene, Camera& camera, float x, float y) override;
	virtual void Unclick() override;

	virtual bool canBeDeleted() const override;
};