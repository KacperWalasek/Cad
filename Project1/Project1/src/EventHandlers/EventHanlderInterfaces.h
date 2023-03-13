#pragma once
#include <GLFW/glfw3.h>

class IMouseEventHandler
{
public:
	virtual void mouseCallback(GLFWwindow* window, int button, int action, int mods) = 0;
};