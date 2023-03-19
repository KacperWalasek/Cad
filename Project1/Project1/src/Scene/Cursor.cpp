#include "Cursor.h"

Cursor::Cursor(Camera& camera)
	:size(10.0f), camera(camera)
{
	mesh.vertices = {
		0.0f,0.0f,0.0f,
		size,0.0f,0.0f,
		0.0f,size,0.0f,
		0.0f,0.0f,size
	};
	mesh.indices = {
		0,1, 0,2, 0,3
	};
	mesh.Init();
}

void Cursor::Render()
{
	mesh.Render();
}

void Cursor::mouseCallback(GLFWwindow* window, int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        switch (action)
        {
        case GLFW_PRESS:
			double posX, posY;
			int sizeX, sizeY;
			glfwGetCursorPos(window, &posX, &posY);
			glfwGetWindowSize(window, &sizeX, &sizeY);

			glm::fvec4 zero(0, 0, 0, 1);
			glm::fvec4 cursorProj = camera.GetProjectionMatrix() * camera.GetViewMatrix() * transform.GetMatrix() * zero;
			cursorProj /= cursorProj.w;

			glm::fvec4 newCursorView(posX/(sizeX/2) - 1.0f, -posY/(sizeY/2) + 1.0f, cursorProj.z, 1.0f);
			glm::fvec4 newCursorWorld = camera.transform.GetMatrix() * camera.GetInverseProjectionMatrix() * newCursorView;
			newCursorWorld /= newCursorWorld.w;
			transform.location = { newCursorWorld.x, newCursorWorld.y, newCursorWorld.z, 0 };
			break;
        }
    }

}
