#pragma once
#include "../interfaces/IGui.h"
#include "../interfaces/ISceneTracker.h"
#include "../interfaces/IRenderable.h"
#include "ISceneElement.h"
#include "Point.h"
#include "BezierCurve.h"
#include "../Rendering/Camera.h"

class CurveC2 : public ISceneElement, public IRenderable, public IGui, public ISceneTracker
{
	std::string name;
	std::vector<BezierCurve> beziers;
	std::vector<std::shared_ptr<Point>> points;
	bool addSelected, removeSelected;
	bool showChain;

	int curveIndicesSize, chainIndicesSize;
	unsigned int VBO, curveEBO, curveVAO, chainEBO, chainVAO;
	Mesh chainMesh;

	Camera& camera;
	Shader shader, deBoorShader;
public:
	CurveC2(Camera& camera, std::vector<std::shared_ptr<Point>> points);

	void UpdateMeshes();
	void UpdateBeziers();

	virtual std::string getName() const override;


	virtual void Render() override;

	virtual void RenderGui() override;


	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onMove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

};