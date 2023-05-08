#pragma once
#include "ISceneElement.h"
#include "../Geometry/TorusGeometry.h"
#include "../Rendering/Mesh.h"
#include "../interfaces/IGui.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/ITransformable.h"
#include "../Rendering/Transform.h"
#include "../Indexer.h"


class Torus : public ISceneElement, public ITransformable, public IGui, public IRenderable
{
	static Indexer indexer;

	TorusGeometry geometry;
	Mesh mesh;
	Transform transform;
	std::string name;
	void UpdateMesh();
public:
	Torus();
	int minorDencity, majorDencity;

	virtual std::string getName() const override
	{
		return name;
	}

	virtual void RenderGui() override;

	virtual Transform& getTransform() override;

	virtual void Render(bool selected, VariableManager& vm) override;

};