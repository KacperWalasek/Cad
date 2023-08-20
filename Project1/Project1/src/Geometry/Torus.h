#pragma once
#include "../interfaces/ISceneElement.h"
#include "../Geometry/TorusGeometry.h"
#include "../Rendering/Mesh.h"
#include "../interfaces/IGui.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/ITransformable.h"
#include "../interfaces/ISerializable.h"
#include "../interfaces/IUVSurface.h"
#include "../Rendering/Transform.h"
#include "../Indexer.h"
#include "Intersection.h"

class Torus : public ISceneElement, public ITransformable, public IGui, public IRenderable, public ISerializable, public IUVSurface
{
	static Indexer indexer;

	TorusGeometry geometry;
	//Mesh mesh;
	unsigned int VAO, VBO, EBO;
	int elementSize;
	Shader shader;

	Transform transform;
	std::string name;

	std::vector<std::weak_ptr<Intersection>> intersections;
	std::vector<unsigned int> intersectionTextures;
	std::vector<bool> intersectEnabled;
	std::vector<bool> intersectReversed;

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

	virtual void Render(bool selected, VariableManager& vm) override;

	virtual nlohmann::json Serialize(Scene& scene, Indexer& indexer, std::map<int, int>& pointIndexMap) const override;


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


	// Inherited via ITransformable
	virtual const Transform& getTransform() const override;

	virtual const void setTransform(const Transform& transform) override;


	// Inherited via ITransformable
	virtual const void setLocation(const glm::fvec3& location) override;

	virtual const void setRotation(const glm::fvec3& rotation) override;

	virtual const void setScale(const glm::fvec3& scale) override;

};