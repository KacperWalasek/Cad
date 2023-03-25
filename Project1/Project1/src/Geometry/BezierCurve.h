#pragma once
#include <glm/glm.hpp>
#include <vector>

class BezierCurve 
{
public:
	std::vector<glm::fvec4> points;
	glm::fvec4 getPointOnCurve(float t) const;
};