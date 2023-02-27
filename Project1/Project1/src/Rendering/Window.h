#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
	GLFWwindow* window;
public:
	bool isOpen() const;
	void Init();
	void Update();
	~Window();
};