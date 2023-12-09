#pragma once
#include "../../interfaces/IUVSurface.h"
#include "../../interfaces/IRenderable.h"
#include <memory>
#include "../../Rendering/Shader.h"

class SurfaceShift : public IUVSurface, public IRenderable {
	float shift;
	bool reverse;
	bool roughing = false;
	const int renderDetails = 20;
	int indicesSize;
	unsigned int VBO, EBO, VAO;
	Shader shader;
	glm::fvec3 normal(float u, float v) const;
	void updateMesh(bool reverse);
	bool smartNormal = false;
public:
	std::shared_ptr<IUVSurface> surface;
	SurfaceShift(std::shared_ptr<IUVSurface> surface, float shift, bool reverse, bool roughing = false);

	// Inherited via IUVSurface
	virtual glm::fvec3 f(float u, float v) const override;

	virtual glm::fvec3 dfdu(float u, float v) const override;

	virtual glm::fvec3 dfdv(float u, float v) const override;

	virtual bool wrappedU() override;

	virtual bool wrappedV() override;

	virtual void acceptIntersection(std::weak_ptr<Intersection> intersection) override;

	virtual void removeIntersection(std::weak_ptr<Intersection> intersection) override;

	virtual const std::vector<std::weak_ptr<Intersection>>& getIntersections() override;

	virtual void setRenderState(SurfaceRenderState state) override;


	// Inherited via IUVSurface
	glm::fvec3 dfduu(float u, float v) const override;

	glm::fvec3 dfduv(float u, float v) const override;

	glm::fvec3 dfdvv(float u, float v) const override;


	// Inherited via IRenderable
	void Render(bool selected, VariableManager& vm) override;
	
	void SmartNormalAssuption(bool smartNormal);
};