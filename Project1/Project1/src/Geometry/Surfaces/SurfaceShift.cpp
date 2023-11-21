#include "SurfaceShift.h"

glm::fvec3 SurfaceShift::normal(float u, float v) const
{
	auto du = surface->dfdu(u, v);
	auto dv = surface->dfdv(u, v);
	return glm::normalize(glm::cross(surface->dfdu(u, v), surface->dfdv(u, v)));
}

SurfaceShift::SurfaceShift(std::shared_ptr<IUVSurface> surface, float shift)
	: surface(surface), shift(shift)
{}

glm::fvec3 SurfaceShift::f(float u, float v) const
{
	glm::fvec3 n = normal(u, v);
	if (n.length() == 0)
		return surface->f(u, v);
	return surface->f(u,v) + normal(u,v) * shift;
}

glm::fvec3 SurfaceShift::dfdu(float u, float v) const
{
	float eps = 0.001f;
	return (surface->f(u, v) - surface->f(u+eps, v))/eps;
}

glm::fvec3 SurfaceShift::dfdv(float u, float v) const
{
	float eps = 0.001f;
	return (surface->f(u, v) - surface->f(u, v + eps)) / eps;
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
