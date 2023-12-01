#pragma once
#include "../../interfaces/IUVSurface.h"
#include <memory>

class SurfaceShift : public IUVSurface {
	float shift;
	bool reverse;
	glm::fvec3 normal(float u, float v) const;
public:
	std::shared_ptr<IUVSurface> surface;
	SurfaceShift(std::shared_ptr<IUVSurface> surface, float shift, bool reverse);

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

};