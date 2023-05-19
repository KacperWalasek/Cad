#pragma once
#include "../../interfaces/IGui.h"
#include "../../interfaces/ISceneModifier.h"
#include "../../Scene/Scene.h"
#include "../../Indexer.h"
#include "SurfaceC0.h"

class SurfaceCreator : public IGui, public ISceneElement, public ISceneModifier, public IRenderable
{
	static Indexer indexer;
	Scene& scene;

	int surfaceCount[2];
	bool cylinder;
	std::string name;

	float sizeX, sizeY;

	std::shared_ptr<SurfaceC0> surface;
	std::shared_ptr<ISceneElement> toRemove;
	std::vector<std::shared_ptr<ISceneElement>> toAdd;
	
	void updateSurface();
	void apply();
public:
	SurfaceCreator(Scene& scene);

	virtual std::string getName() const override;
	virtual bool RenderGui() override;

	virtual std::vector<std::shared_ptr<ISceneElement>> GetAddedObjects() override;
	virtual std::vector<std::shared_ptr<ISceneElement>> GetRemovedObjects() override;

	virtual void Render(bool selected, VariableManager& vm) override;

};