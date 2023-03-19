#include "CameraMovement.h"
#include <imgui/imgui.h>
#include <iostream>

CameraMovement::CameraMovement(Camera& camera)
	:camera(camera)
{
}

void CameraMovement::mouseCallback(GLFWwindow* window, int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        switch (action)
        {
        case GLFW_PRESS:
            glfwGetCursorPos(window, &initialMousePosX, &initialMousePosY);
            stableTransform = camera.transform;
            tmpTransform = {};
            break;
        case GLFW_RELEASE:
            curentMouseVectorX = 0;
            curentMouseVectorY = 0;
            shouldApply = true;
            camera.transform = stableTransform + tmpTransform;
            break;
        }
    }

}

void CameraMovement::Update(GLFWwindow* window)
{
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
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

        if (isCrtlPressed)
        {
            float scale = fmax(0, 100 - curentMouseVectorX) / 100;
            tmpTransform.scale = {scale,scale,scale,0};

        }
        else if (isShiftPressed)
        {
            tmpTransform.location = glm::fvec4(-curentMouseVectorX / 10, curentMouseVectorY / 10, 0.0f, 0.0f);
        }
        else
        {
            tmpTransform.rotation = glm::fvec4(curentMouseVectorY / 1000, -curentMouseVectorX / 1000, 0.0f, 0.0f);
        }
        camera.transform = stableTransform + tmpTransform;
    }
}
