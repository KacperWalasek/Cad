#pragma once
#include "ISceneElement.h"
#include "../Geometry/TorusGeometry.h"
#include "../Rendering/Mesh.h"
#include "../interfaces/IGui.h"
#include "../interfaces/IMeshOwner.h"

class Torus : public ISceneElement, public IGui, public IMeshOwner
{
	TorusGeometry geometry;
	Mesh mesh;
	void UpdateMesh();
public:
	Torus();
	int minorDencity, majorDencity;

	virtual std::string getName() const override
	{
		return "Tours";
	}

	virtual void RenderGui() override;

	virtual Mesh& getMesh() override;

};