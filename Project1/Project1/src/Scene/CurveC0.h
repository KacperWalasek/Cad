#pragma once
#include "ISceneElement.h"
#include "../interfaces/IRenderable.h"
#include "ISceneElement.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/IGui.h"
#include "../interfaces/ISceneTracker.h"
#include "../Geometry/BezierCurve.h"
#include "Point.h"
#include <memory>
#include "../Rendering/Camera.h"
#include "../Rendering/Shader.h"

class CurveC0 : public ISceneElement, public IRenderable, public IGui, public ISceneTracker
{
	std::string name;
	Mesh curveMesh;
	Mesh chainMesh;
	std::vector<BezierCurve> beziers;
	std::vector<std::shared_ptr<Point>> points;
	bool addSelected, removeSelected;
	bool showChain;

	Camera& camera;
	Shader shader;
public:
	CurveC0(Camera& camera);
	CurveC0(Camera& camera, std::vector<std::shared_ptr<Point>> points);

	void UpdateMeshes();

	virtual std::string getName() const override;

	virtual void Render() override;

	virtual void RenderGui() override;

	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) override;


};