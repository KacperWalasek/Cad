#include "CameraMainPositions.h"
#include <iostream>
#include <numbers>

CameraMainPositions::CameraMainPositions(Camera& camera)
	: camera(camera)
{
}

void CameraMainPositions::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;

	switch (key)
	{
	case GLFW_KEY_KP_1:
		camera.transform.rotation.xyz = { 0 };
		break;
	case GLFW_KEY_KP_2:
		camera.transform.rotation.xyz = { 0, std::numbers::pi / 2.0f, 0 };
		break;
	case GLFW_KEY_KP_3:
		camera.transform.rotation.xyz = { std::numbers::pi / 2.0f, 0, 0 };
		break;
	default:
		break;
	}
}
