#pragma once
#define GLM_SWIZZLE
#include <glm/glm.hpp>

class TorusGeometry
{
public:
	float majorR;
	float minorR;
	TorusGeometry();
	TorusGeometry(float majorR, float minorR);

	glm::fvec4 getPointOnMesh(float majorAngle, float minorAngle) const;
};