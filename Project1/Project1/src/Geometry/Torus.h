#pragma once
#include "../Math/Vector4.h"
class Torus 
{
	float majorR;
	float minorR;
public:
	Torus();
	Torus(float majorR, float minorR);

	CadMath::Vector4 getPointOnMesh(float majorAngle, float minorAngle) const;
};