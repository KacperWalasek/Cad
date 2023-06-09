#pragma once
#include <glm/glm.hpp>

class Rotator 
{
public:
	static glm::fmat4x4 GetRotationMatrix(glm::fvec4 vector, float angle);
	static glm::fvec4 AxisToEuler(glm::fvec4 vector, float angle);
	static glm::fvec4 MatrixToEuler(glm::fmat4x4 r);
	static float RadToDeg(float rad);
	static float DegToRad(float deg);
};