#pragma once
#include "../../interfaces/IUVSurface.h"
#include <memory>

class SurfaceShift : public IUVSurface {
	std::shared_ptr<IUVSurface> surface;
	float shift;
	glm::fvec3 normal(float u, float v) const;
public:
	SurfaceShift(std::shared_ptr<IUVSurface> surface, float shift);

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

};