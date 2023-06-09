#pragma once
#include "../../interfaces/IPointOwner.h"
#include "../../interfaces/ISceneElement.h"
#include "../../interfaces/IGui.h"
#include "../../interfaces/IRenderable.h"
#include "../../interfaces/ISceneTracker.h"
#include "../../Indexer.h"
#include "../Point.h"
#include "../../Rendering/Shader.h"

struct Patch
{
	std::shared_ptr<ISceneElement> surface;
	std::shared_ptr<Point> points[4][4];
	std::shared_ptr<Point> corners[4];
};
struct Hole
{
	std::array<Patch, 3> patches;
	std::array<int, 3> corners;
};

class GregoryPatch : public ISceneElement, public IOwner, public IGui, public IRenderable, public ISceneTracker
{
	static Indexer indexer;

	
	std::string name;
	std::array<std::array<glm::fvec3, 20>, 3> subPatches;
	/*
	0    1     2    3
	4   5,6   7,8   9
	10 11,12 13,14 15
	16  17    18   19
	*/

	Shader meshShader, chainShader;
	unsigned int meshVAO, meshVBO, meshEBO, chainVAO, chainEBO;

	int chainIndsSize, meshIndsSize;
	
	int division[2];
	bool shouldReload, showChain;

	std::array<std::array<std::pair<int, int>, 4>, 3> orderCorners();

	glm::fvec3 deCastljeu(float t, std::array<glm::fvec3, 4> p);
	glm::fvec3 edgeDeCasteljeu(float t, Patch& p, std::pair<int, int> c1, std::pair<int, int> c2);
	glm::fvec3 derivative(float t, Patch& p, std::pair<int, int> c1, std::pair<int, int> c2);

	void updateMeshes();
	void createBuffers();
public:
	Hole hole;
	static std::array<std::pair<int, int>, 4> cornerIndices;
	static glm::fvec3 pointOnEdge(int i, Patch& p, std::pair<int, int> c1, std::pair<int, int> c2);

	GregoryPatch(Hole hole);

	// Inherited via ISceneElement
	virtual std::string getName() const override;

	// Inherited via IPointOwner
	virtual void SelectAll(Scene& scene) const override;
	virtual bool CanChildBeDeleted() const override;

	// Inherited via IGui
	virtual bool RenderGui() override;

	// Inherited via IRenderable
	virtual void Render(bool selected, VariableManager& vm) override;

	// Inherited via ISceneTracker
	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onMove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onCollapse(Scene& scene, std::vector<std::shared_ptr<Point>>& collapsed, std::shared_ptr<Point> result) override;
};