#pragma once
#include "Mesh.h"
#include "../Geometry/TorusGeometry.h"

class Sampler 
{
public:
	Mesh sampleTorus(const TorusGeometry& torus, int majorDivision, int minorDivision) const;
};