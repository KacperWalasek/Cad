#pragma once
#include "ISceneElement.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/IGui.h"
#include "../interfaces/ISceneTracker.h"
#include "../Geometry/BezierCurve.h"
#include "Point.h"
#include <memory>

class Curve : public ISceneElement, public IRenderable, public IGui, public ISceneTracker
{
	Transform transform; // DO USUNIECIA
	std::string name;
	Mesh mesh;
	std::vector<BezierCurve> beziers;
	std::vector<std::shared_ptr<Point>> points;
public:
	Curve();
	Curve(std::vector<std::shared_ptr<Point>> points);

	virtual std::string getName() const override;

	virtual Transform& getTransform() override;

	virtual void Render() override;

	virtual void RenderGui() override;

	void UpdateMeshes();


	// Inherited via ISceneTracker
	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;


	// Inherited via ISceneTracker
	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

};