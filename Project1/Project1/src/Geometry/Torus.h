#pragma once
#include <glm/glm.hpp>

class Torus 
{
	float majorR;
	float minorR;
public:
	Torus();
	Torus(float majorR, float minorR);

	glm::fvec4 getPointOnMesh(float majorAngle, float minorAngle) const;
};