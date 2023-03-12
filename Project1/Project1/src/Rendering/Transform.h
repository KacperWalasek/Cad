#pragma once
#include "glm/glm.hpp"
struct Transform
{ 
	Transform()
		: location(0,0,0,0), rotation(0,0,0,0), scale(1,1,1,0)
	{}
	Transform(glm::fvec4 location, glm::fvec4 rotation, glm::fvec4 scale)
		: location(location), rotation(rotation), scale(scale)
	{}

	glm::fvec4 location;
	glm::fvec4 rotation;
	glm::fvec4 scale;

	glm::fmat4x4 GetMatrix() const;
	glm::fmat4x4 GetInverseMatrix() const;
	glm::fmat4x4 GetRotationMatrix() const;
	glm::fmat4x4 GetLocationMatrix() const;
	glm::fmat4x4 GetScaleMatrix() const;
	
	Transform operator+ (const Transform& t);
};