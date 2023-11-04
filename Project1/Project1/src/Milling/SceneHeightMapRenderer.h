#pragma once
#include "../interfaces/IRenderable.h"
#include "../interfaces/IUVSurface.h"
#include "../Scene/Scene.h"

class SceneHeightMapRenderer : public IRenderable
{
	std::vector<std::shared_ptr<IUVSurface>> surfaces;
public:
	SceneHeightMapRenderer(Scene& scene);

	virtual void Render(bool selected, VariableManager& vm) override;

};