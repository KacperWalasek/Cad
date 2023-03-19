#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class IMouseEventHandler
{
public:
	virtual void mouseCallback(GLFWwindow* window, int button, int action, int mods) = 0;
};

class IKeyEventHandler
{
public:
	virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
};