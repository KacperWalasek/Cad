#pragma once
#include "../../interfaces/ISceneElement.h"
#include "../../interfaces/IPointOwner.h"
#include "../../interfaces/ISerializable.h"
#include "../../Indexer.h"
#include "../Point.h"
#include "../../Rendering/Shader.h"
#include <vector>
#include <memory>

class SurfaceC2 : public ISceneElement, public IRenderable, public ISceneTracker, public IOwner, public IGui, public ISerializable
{
	static Indexer indexer;

	std::string name;

	unsigned int VBO, EBO, VAO;
	Shader tessShader, chainShader;
	void updateMeshes();

	void CreatePointsFlat();
	void CreatePointsCylinder();

	int indicesSize;

	glm::fvec4 pos;
	int division[2];
	bool showChain;

	bool shouldReload;

	std::vector<glm::fvec4> positions;
	SurfaceC2();
public:
	float sizeX, sizeY;
	int countX, countY;
	bool cylinder;

	std::vector<std::shared_ptr<Point>> points;

	SurfaceC2(glm::fvec4 pos, int countX, int countY, float sizeX, float sizeY, bool cylinder);
	SurfaceC2(nlohmann::json json, std::map<int, std::shared_ptr<Point>>& pointMap);

	virtual std::string getName() const override;

	virtual void Render(bool selected, VariableManager& vm) override;

	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onMove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual bool CanChildBeDeleted() const override;

	// Inherited via IGui
	virtual bool RenderGui() override;

	// Inherited via IPointOwner
	virtual void SelectAll(Scene& scene) const override;


	void Recalculate();
	void CreateControlPoints();

	// Inherited via ISerializable
	virtual nlohmann::json Serialize(Scene& scene, Indexer& indexer, std::map<int, int>& pointIndexMap) const override;

	// Inherited via ISceneTracker
	virtual void onCollapse(Scene& scene, std::vector<std::shared_ptr<Point>>& collapsed, std::shared_ptr<Point> result) override;
};