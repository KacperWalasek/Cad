#pragma once
#include "../interfaces/ISceneElement.h"
#include "../Geometry/TorusGeometry.h"
#include "../Rendering/Mesh.h"
#include "../interfaces/IGui.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/ITransformable.h"
#include "../interfaces/ISerializable.h"
#include "../Rendering/Transform.h"
#include "../Indexer.h"


class Torus : public ISceneElement, public ITransformable, public IGui, public IRenderable, public ISerializable
{
	static Indexer indexer;

	TorusGeometry geometry;
	Mesh mesh;
	Transform transform;
	std::string name;
	void UpdateMesh();
public:
	Torus();
	Torus(nlohmann::json json);
	int minorDencity, majorDencity;

	virtual std::string getName() const override
	{
		return name;
	}

	virtual bool RenderGui() override;

	virtual Transform& getTransform() override;

	virtual void Render(bool selected, VariableManager& vm) override;

	virtual nlohmann::json Serialize(Scene& scene, Indexer& indexer, std::map<int, int>& pointIndexMap) const override;

};