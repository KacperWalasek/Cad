#pragma once
#include "../interfaces/IGui.h"
#include "../interfaces/ISceneTracker.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/ICustomMove.h"
#include "ISceneElement.h"
#include "Point.h"
#include "BezierCurve.h"
#include "../Rendering/Camera.h"
#include <tuple>

class CurveC2 : public ISceneElement, public IRenderable, public IGui, public ISceneTracker, public IClickable, public ICustomMove
{
	std::string name;
	std::vector<BezierCurve> beziers;
	std::vector<Point> bezierPoints;
	int selectedBezier;
	std::vector<std::shared_ptr<Point>> points;
	bool addSelected, removeSelected;
	bool showChain;
	
	glm::fvec4 movementStartPosition;

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


	virtual void Render(bool selected) override;

	virtual void RenderGui() override;


	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual void onMove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;


	std::tuple<int,float> getClickIndex(Camera& camera, float x, float y) const;

	virtual std::tuple<bool, float> InClickRange(Camera& camera, float x, float y) const override;

	virtual bool Click(Scene& scene, Camera& camera, float x, float y) override;


	// Inherited via ICustomMove
	virtual bool Translate(glm::fvec4 translation) override;

	virtual bool Rotate(glm::fvec4 rotation) override;

	virtual bool Scale(glm::fvec4 scale) override;



	// Inherited via ICustomMove
	virtual void StartMove() override;

};