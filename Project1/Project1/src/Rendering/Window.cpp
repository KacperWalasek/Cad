#include "Window.h"
#include <iostream>
#include <stdexcept>
bool Window::isOpen() const
{
	return !glfwWindowShouldClose(window);
}

void Window::Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(1600, 900, "Cad", NULL, NULL);
    if (window == NULL)
        throw std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(window);
}

void Window::Update()
{
    glfwSwapBuffers(window);
    glfwPollEvents();
}

Window::~Window()
{
    glfwTerminate();
}
