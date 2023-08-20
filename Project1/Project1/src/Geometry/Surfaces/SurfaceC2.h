#pragma once
#include "../../interfaces/ISceneElement.h"
#include "../../interfaces/IPointOwner.h"
#include "../../interfaces/ISerializable.h"
#include "../../interfaces/IUVSurface.h"
#include "../../Indexer.h"
#include "../Point.h"
#include "../../Rendering/Shader.h"
#include <vector>
#include <memory>
#include "../Intersection.h"

class SurfaceC2 : public ISceneElement, public IRenderable, public ISceneTracker, public IOwner, public IGui, public ISerializable, public IUVSurface
{
	static Indexer indexer;

	std::string name;

	unsigned int VBO, EBO, VAO;
	Shader tessShader, chainShader;
	void updateMeshes();

	void CreatePointsFlat();
	void CreatePointsCylinder();
	int pointIndex(int sX, int sY, int pX, int pY) const;

	int indicesSize;

	glm::fvec4 pos;
	int division[2];
	bool showChain;

	bool shouldReload;
	bool mirrorU = false, mirrorV = false, anchor = false;

	std::vector<std::weak_ptr<Intersection>> intersections;
	std::vector<unsigned int> intersectionTextures;
	std::vector<bool> intersectEnabled;
	std::vector<bool> intersectReversed;

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

	// Inherited via IUVSurface
	virtual glm::fvec3 f(float u, float v) const override;
	virtual glm::fvec3 dfdu(float u, float v) const override;
	virtual glm::fvec3 dfdv(float u, float v) const override;

	// Inherited via IUVSurface
	virtual bool wrappedU() override;
	virtual bool wrappedV() override;

	// Inherited via IUVSurface
	virtual void acceptIntersection(std::weak_ptr<Intersection> intersection) override;
	virtual void removeIntersection(std::weak_ptr<Intersection> intersection) override;

	// Inherited via IOwner
	virtual bool CanChildBeMoved() const override;

	// Inherited via IOwner
	virtual void ChildMoved(ISceneElement& child) override;
};