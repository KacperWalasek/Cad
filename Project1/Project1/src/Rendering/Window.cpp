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


    window = glfwCreateWindow(1600, 900, "Cad", NULL, NULL);
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
}

void Window::Update()
{
    ProcessMouseInput();
}

void Window::ProcessMouseInput()
{
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
        return;
    }
    isCrtlPressed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    isShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        lastTouchTime = glfwGetTime();
        double posX, posY;
        glfwGetCursorPos(window, &posX, &posY);
        curentMouseVectorX = posX - initialMousePosX;
        curentMouseVectorY = posY - initialMousePosY;
        std::cout << curentMouseVectorX << ", " << curentMouseVectorY << std::endl;
    }
}


void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        Window* windowObject = (Window*)glfwGetWindowUserPointer(window);
        switch (action)
        {
        case GLFW_PRESS:
            glfwGetCursorPos(window, &(windowObject->initialMousePosX), &(windowObject->initialMousePosY));
            break;
        case GLFW_RELEASE:
            windowObject->curentMouseVectorX = 0;
            windowObject->curentMouseVectorY = 0;
            windowObject->shouldApply = true;
            break;
        }
    }
}
