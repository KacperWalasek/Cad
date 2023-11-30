#include "SurfaceShift.h"

glm::fvec3 SurfaceShift::normal(float u, float v) const
{
	auto du = surface->dfdu(u, v);
	auto dv = surface->dfdv(u, v);
	if (glm::length(du) == 0 || glm::length(dv) == 0)
		return { 0,0,0 };
	return glm::normalize(glm::cross(surface->dfdu(u, v), surface->dfdv(u, v)));
}

SurfaceShift::SurfaceShift(std::shared_ptr<IUVSurface> surface, float shift, bool reverse)
	: surface(surface), shift(shift), reverse(reverse)
{}

glm::fvec3 SurfaceShift::f(float u, float v) const
{
	glm::fvec3 n = normal(u, v);
	if (reverse)
		n = -n;
	return surface->f(u,v) + n * shift;
}

glm::fvec3 SurfaceShift::dfdu(float u, float v) const
{
	return surface->dfdu(u,v);
	float eps = 0.0001f;
	return (surface->f(u + eps, v) - surface->f(u, v))/eps;
}

glm::fvec3 SurfaceShift::dfdv(float u, float v) const
{
	return surface->dfdv(u, v);
	float eps = 0.0001f;
	return (surface->f(u, v + eps) - surface->f(u, v)) / eps;
}

bool SurfaceShift::wrappedU()
{
	return surface->wrappedU();
}

bool SurfaceShift::wrappedV()
{
	return surface->wrappedV();
}

void SurfaceShift::acceptIntersection(std::weak_ptr<Intersection> intersection)
{
	return surface->acceptIntersection(intersection);
}

void SurfaceShift::removeIntersection(std::weak_ptr<Intersection> intersection)
{
	return surface->removeIntersection(intersection);
}

const std::vector<std::weak_ptr<Intersection>>& SurfaceShift::getIntersections()
{
	return surface->getIntersections();
}

void SurfaceShift::setRenderState(SurfaceRenderState state)
{
	return surface->setRenderState(state);
}
