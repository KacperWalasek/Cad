#pragma once
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <vector>
#include <memory>

enum class SurfaceRenderState {
	Wireframe,
	Solid,
	HeightMap
};

class Intersection;
class IUVSurface
{
public:
	virtual glm::fvec3 f(float u, float v) const = 0;
	virtual glm::fvec3 dfdu(float u, float v) const = 0;
	virtual glm::fvec3 dfdv(float u, float v) const = 0;

	virtual bool wrappedU() = 0;
	virtual bool wrappedV() = 0;

	virtual void acceptIntersection(std::weak_ptr<Intersection> intersection) = 0;
	virtual void removeIntersection(std::weak_ptr<Intersection> intersection) = 0;
	virtual const std::vector<std::weak_ptr<Intersection>>& getIntersections() = 0;

	virtual void setRenderState(SurfaceRenderState state) = 0;
};