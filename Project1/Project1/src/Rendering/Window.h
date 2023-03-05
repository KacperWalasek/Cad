#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

class Window
{

public:
	GLFWwindow* window;
	bool isCrtlPressed = false;
	bool isShiftPressed = false;
	bool shouldApply = false;
	double initialMousePosX = 0;
	double initialMousePosY = 0;
	double curentMouseVectorX = 0;
	double curentMouseVectorY = 0;
	float lastTouchTime = 0; 
	bool isOpen() const;
	void Init();
	void SetCallbacks();
	void Update();
	void ProcessMouseInput();
	~Window();
};

void mouseCallback(GLFWwindow* window, int button, int action, int mods);