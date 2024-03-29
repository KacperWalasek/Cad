#pragma once
#include "../interfaces/ISceneElement.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/ITransformable.h"
#include <memory>

class Center : public IRenderable
{
	Mesh mesh;
	float size;
public:
	Transform transform;
	Center();

	virtual void Render(bool selected, VariableManager& vm) override;
	void UpdateTransform(std::vector<std::pair<std::shared_ptr<ISceneElement>, bool>>& objects);
};