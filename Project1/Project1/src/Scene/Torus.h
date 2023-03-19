#pragma once
#include "ISceneElement.h"
#include "../Geometry/TorusGeometry.h"
#include "../Rendering/Mesh.h"
#include "../interfaces/IGui.h"
#include "../interfaces/IRenderable.h"
#include "../Rendering/Transform.h"

class Torus : public ISceneElement, public IGui, public IRenderable
{
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

	virtual void Render() override;

};