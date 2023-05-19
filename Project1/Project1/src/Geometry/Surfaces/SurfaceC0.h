#pragma once
#include "../../interfaces/ISceneElement.h"
#include "../../interfaces/IPointOwner.h"
#include "../../Indexer.h"
#include "../Point.h"
#include "../../Rendering/Shader.h"
#include <vector>
#include <memory>

class SurfaceC0 : public ISceneElement, public IRenderable, public ISceneTracker, public IPointOwner, public IGui
{
	static Indexer indexer;

	std::string name;

	unsigned int VBO, EBO, VAO;
	Shader tessShader,  chainShader;
	void updateMeshes();

	void CreatePointsFlat();
	void CreatePointsCylinder();
	void CreateSmiglo();
	
	int indicesSize;

	glm::fvec4 pos;
	int division[2];
	bool showChain;

	bool shouldReload;
public:
	float sizeX, sizeY;
	int countX, countY;
	bool cylinder;

	std::vector<std::shared_ptr<Point>> points;

	SurfaceC0(glm::fvec4 pos, int countX, int countY, float sizeX, float sizeY, bool cylinder);

	virtual std::string getName() const override;

	virtual void Render(bool selected, VariableManager& vm) override;

	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onMove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;

	virtual bool CanBeDeleted(const Point& p) const override;

	// Inherited via IGui
	virtual bool RenderGui() override;

	// Inherited via IPointOwner
	virtual void SelectAll(Scene& scene) const override;


	void Recalculate();
};