#pragma once
#include "ISceneElement.h"
#include "../interfaces/IRenderable.h"
#include "ISceneElement.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/IGui.h"
#include "../interfaces/ISceneTracker.h"
#include "../interfaces/ICustomMove.h"
#include "Point.h"
#include <memory>
#include "../Rendering/Camera.h"
#include "../Rendering/Shader.h"
#include "BezierCurve.h"
#include "../Indexer.h"

class InterpolationCurve : public ISceneElement, public IRenderable, public IGui, public ISceneTracker
{
	static Indexer indexer;
	std::string name;
	std::vector<std::shared_ptr<Point>> points;
	std::vector<BezierCurve> beziers;
	bool addSelected, removeSelected;
	bool showChain;
	bool chord;

	Shader shader;
public:
	InterpolationCurve(std::vector<std::shared_ptr<Point>> points);

	void UpdateMeshes();

	virtual std::string getName() const override;

	virtual void Render(bool selected, VariableManager& vm) override;

	virtual void RenderGui() override;

	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onMove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	std::tuple<int, float> getClickIndex(Camera& camera, float x, float y) const;

};