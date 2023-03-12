#pragma once 
#include "Transform.h"
#include <glm/glm.hpp>
class Camera
{
	Transform transform;
public:
	glm::fmat4x4 GetViewMatrix() const;
	
};