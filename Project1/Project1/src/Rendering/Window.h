#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include <vector>
#include "../EventHandlers/EventHanlderInterfaces.h"

class Window
{

public:
	float sizeX;
	float sizeY;
	std::vector<std::shared_ptr<IMouseEventHandler>>  mouseCallbacks;
	std::vector<std::shared_ptr<IKeyEventHandler>>  keyCallbacks;
	GLFWwindow* window;
	bool isOpen() const;
	void Init();
	void SetCallbacks();
	void Update();
	void ProcessMouseInput();
	Window();
	~Window();
};

void mouseCallback(GLFWwindow* window, int button, int action, int mods);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);