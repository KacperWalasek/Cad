#pragma once
#include "Mesh.h"
#include "../Geometry/TorusGeometry.h"
#include "../Geometry/BezierCurve.h"

class Sampler 
{
public:
	Mesh sampleTorus(const TorusGeometry& torus, int majorDivision, int minorDivision) const;
	Mesh sampleBezierCurve(const BezierCurve& curve, int division);
};