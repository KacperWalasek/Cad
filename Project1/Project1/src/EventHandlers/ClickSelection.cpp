#include "ClickSelection.h"
#include "../Scene/Point.h"

ClickSelection::ClickSelection(Scene& scene, Camera& camera)
	:scene(scene), camera(camera)
{
}

void ClickSelection::mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS && glfwGetKey(window,GLFW_KEY_P) == GLFW_PRESS)
	{
		glm::dvec2 currentPos;
		int sizeX, sizeY;
		glfwGetCursorPos(window, &currentPos.x, &currentPos.y);
		glfwGetWindowSize(window, &sizeX, &sizeY);

		glm::fvec2 cursorPos(currentPos.x / (sizeX / 2) - 1.0f, -currentPos.y / (sizeY / 2) + 1.0f);

		float bestZ = 1;
		std::shared_ptr<Point> selected = nullptr;
		for (auto& obj : scene.objects)
		{
			std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(obj);
			if (!p)
				continue;
			glm::fvec4 screenPosition = camera.GetProjectionMatrix() * camera.GetViewMatrix()*p->getTransform().GetMatrix()* glm::fvec4(0, 0, 0, 1);
			screenPosition /= screenPosition.w;
			float dist2 = pow(cursorPos.x - screenPosition.x, 2) + pow(cursorPos.y - screenPosition.y, 2);
			if (screenPosition.z < bestZ && dist2 < 0.0001f)
			{
				bestZ = screenPosition.z;
				selected = p;
			}
		}
		if (selected)
			scene.selected = selected;
	}
}
