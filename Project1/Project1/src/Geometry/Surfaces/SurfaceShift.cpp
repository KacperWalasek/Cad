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
	glm::fvec3 sdfdu = surface->dfdu(u, v);
	glm::fvec3 sdfdv = surface->dfdv(u, v);
	glm::fvec3 sdfduu = surface->dfduu(u, v);
	glm::fvec3 sdfduv = surface->dfduv(u, v);
	glm::fvec3 sdfdvv = surface->dfdvv(u, v);

	glm::fvec3 norm = glm::cross(sdfdu, sdfdv);
	float normnorm = glm::length(norm);
	glm::fvec3 normU = glm::cross(sdfduu, sdfdv) + glm::cross(sdfdu, sdfduv);
	return sdfdu;//  +(normU / normnorm - norm * glm::dot(norm, normU) / (normnorm * normnorm * normnorm)) * shift;
}

glm::fvec3 SurfaceShift::dfdv(float u, float v) const
{
	glm::fvec3 sdfdu = surface->dfdu(u, v);
	glm::fvec3 sdfdv = surface->dfdv(u, v);
	glm::fvec3 sdfduu = surface->dfduu(u, v);
	glm::fvec3 sdfduv = surface->dfduv(u, v);
	glm::fvec3 sdfdvv = surface->dfdvv(u, v);

	glm::fvec3 norm = glm::cross(sdfdu, sdfdv);
	float normnorm = glm::length(norm);
	glm::fvec3 normV = glm::cross(sdfduv, sdfdv) + glm::cross(sdfdu, sdfdvv);
	return sdfdv + (normV / normnorm - norm * glm::dot(norm, normV) / (normnorm * normnorm * normnorm))*shift;
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

glm::fvec3 SurfaceShift::dfduu(float u, float v) const
{
	return surface->dfduu(u, v);
}

glm::fvec3 SurfaceShift::dfduv(float u, float v) const
{
	return surface->dfduv(u, v);
}

glm::fvec3 SurfaceShift::dfdvv(float u, float v) const
{
	return surface->dfdvv(u, v);
}
