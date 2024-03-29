#pragma once
#include <memory>
#include "BezierCurve.h"
#include "../../interfaces/ISceneElement.h"
#include "../../interfaces/IRenderable.h"
#include "../../interfaces/IRenderable.h"
#include "../../interfaces/IGui.h"
#include "../../interfaces/ISceneTracker.h"
#include "../../interfaces/ICustomMove.h"
#include "../../interfaces/ISerializable.h"
#include "../../interfaces/IPointOwner.h"
#include "../Point.h"
#include "../../Rendering/Camera.h"
#include "../../Rendering/Shader.h"
#include "../../Indexer.h"

class InterpolationCurve : public ISceneElement, public IRenderable, public IGui, 
	public ISceneTracker, public ISerializable, public IOwner
{
	static Indexer indexer;
	std::string name;
	std::vector<std::shared_ptr<Point>> points;
	std::vector<BezierCurve> beziers;
	bool addSelected, removeSelected;
	bool showChain;
	bool chord;

	Shader shader;
	InterpolationCurve();
public:
	InterpolationCurve(std::vector<std::shared_ptr<Point>> points);
	InterpolationCurve(nlohmann::json json,  std::map<int, std::shared_ptr<Point>>& pointMap);

	void UpdateMeshes();

	virtual std::string getName() const override;

	virtual void Render(bool selected, VariableManager& vm) override;

	virtual bool RenderGui() override;

	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onMove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	std::tuple<int, float> getClickIndex(Camera& camera, float x, float y) const;


	// Inherited via ISerializable
	virtual nlohmann::json Serialize(Scene& scene, Indexer& indexer, std::map<int, int>& pointIndexMap) const override;


	// Inherited via ISceneTracker
	virtual void onCollapse(Scene& scene, std::vector<std::shared_ptr<Point>>& collapsed, std::shared_ptr<Point> result) override;


	// Inherited via IOwner
	virtual void SelectAll(Scene& scene) const override;

	virtual bool CanChildBeDeleted() const override;


	// Inherited via IOwner
	virtual bool CanChildBeMoved() const override;


	// Inherited via IOwner
	virtual void ChildMoved(ISceneElement& child) override;

};