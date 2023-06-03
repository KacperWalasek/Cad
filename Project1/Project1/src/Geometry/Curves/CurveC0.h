#pragma once
#include <memory>
#include "../../interfaces/ISceneElement.h"
#include "../../interfaces/IRenderable.h"
#include "../../interfaces/IGui.h"
#include "../../interfaces/ISceneTracker.h"
#include "../../interfaces/ISerializable.h"
#include "../Point.h"
#include "../../Rendering/Camera.h"
#include "../../Rendering/Shader.h"
#include "BezierCurve.h"
#include "../../Indexer.h"

class CurveC0 : public ISceneElement, public IRenderable, public IGui, public ISceneTracker, public ISerializable
{
	static Indexer indexer;
	std::string name;
	std::vector<std::shared_ptr<Point>> points;
	std::vector<BezierCurve> beziers;
	bool addSelected, removeSelected;
	bool showChain;

	Shader shader;
	CurveC0();
public:
	CurveC0(std::vector<std::shared_ptr<Point>> points);
	CurveC0(nlohmann::json json, std::map<int, std::shared_ptr<Point>>& pointMap);

	void UpdateMeshes();

	virtual std::string getName() const override;

	virtual void Render(bool selected, VariableManager& vm) override;

	virtual bool RenderGui() override;

	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onMove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual nlohmann::json Serialize(Scene& scene, Indexer& indexer, std::map<int, int>& pointIndexMap) const override;

};