#pragma once
#include "Mesh.h"
#include "../Geometry/Torus.h"

class Sampler 
{
public:
	Mesh sampleTorus(const Torus& torus, int majorDivision, int minorDivision) const;
};