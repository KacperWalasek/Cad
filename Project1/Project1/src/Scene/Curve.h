#pragma once
#include "ISceneElement.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/IGui.h"
#include "../interfaces/ISceneTracker.h"
#include "../Geometry/BezierCurve.h"
#include "Point.h"
#include <memory>
#include "../Rendering/Camera.h"

class Curve : public ISceneElement, public IRenderable, public IGui, public ISceneTracker
{
	const int minDivision = 5;
	const int maxDivision = 150;
	const float divisionMultiplyer = 50.0f;

	std::string name;
	Mesh mesh;
	Mesh chainMesh;
	std::vector<BezierCurve> beziers;
	std::vector<std::shared_ptr<Point>> points;
	bool addSelected, removeSelected;
	bool showChain;

	Camera& camera;

	int calculateDivision(BezierCurve& bezier);
public:
	Curve(Camera& camera);
	Curve(Camera& camera,std::vector<std::shared_ptr<Point>> points);

	virtual std::string getName() const override;

	virtual void Render() override;

	virtual void RenderGui() override;

	void UpdateMeshes();

	// Inherited via ISceneTracker
	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

};