#include "Window.h"
#include <iostream>
#include <stdexcept>

bool Window::isOpen() const
{
	return !glfwWindowShouldClose(window);
}

Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void Window::Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

 
    window = glfwCreateWindow(sizeX, sizeY, "Cad", NULL, NULL);
    if (window == NULL)
        throw std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    SetCallbacks();


    ImGui::CreateContext();
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
}

void Window::SetCallbacks()
{
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);
}

void Window::Update()
{
    ProcessMouseInput();
}

void Window::ProcessMouseInput()
{
}

Window::Window()
    : sizeX(1600), sizeY(900)
{
}


void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    Window* windowObject = (Window*)glfwGetWindowUserPointer(window);
    for (auto& handler : windowObject->mouseCallbacks)
        handler->mouseCallback(window, button, action, mods);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Window* windowObject = (Window*)glfwGetWindowUserPointer(window);
    for (auto& handler : windowObject->keyCallbacks)
        handler->keyCallback(window, key, scancode, action, mods);
}
