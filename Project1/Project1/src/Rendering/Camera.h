#pragma once 
#include "Transform.h"
#include <glm/glm.hpp>
#include "Window.h"
class Camera
{
	Window& window;
public:
	float fov;
	float n;
	float f;

	Camera(Window& window, float fov, float n, float f);
	Transform transform;
	glm::fmat4x4 GetViewMatrix() const;
	glm::fmat4x4 GetProjectionMatrix() const;
};