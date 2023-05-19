#include "ClickSelection.h"
#include "../Geometry/Point.h"

ClickSelection::ClickSelection(Scene& scene, Camera& camera)
	:scene(scene), camera(camera)
{
}

void ClickSelection::mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		glm::dvec2 currentPos;
		int sizeX, sizeY;
		glfwGetCursorPos(window, &currentPos.x, &currentPos.y);
		glfwGetWindowSize(window, &sizeX, &sizeY);

		glm::fvec2 cursorPos(currentPos.x / (sizeX / 2) - 1.0f, -currentPos.y / (sizeY / 2) + 1.0f);

		float bestZ = 1;
		std::pair<std::shared_ptr<ISceneElement>, bool>* selected = nullptr;
		for (auto& obj : scene.objects)
		{
			std::shared_ptr<IClickable> clickable = std::dynamic_pointer_cast<IClickable>(obj.first);
			if (!clickable)
				continue;
			auto [inRange, clickZ] = clickable->InClickRange(camera, cursorPos.x, cursorPos.y);
			if (inRange && clickZ < bestZ)
			{
				bestZ = clickZ;
				selected = &obj;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
			for (auto& obj : scene.objects)
			{
				obj.second = false;
				std::shared_ptr<IClickable> clickable = std::dynamic_pointer_cast<IClickable>(obj.first);
				if (clickable)
					clickable->Unclick();
			}
		if (selected)
		{
			if(std::dynamic_pointer_cast<IClickable>(selected->first)->Click(scene, camera, cursorPos.x, cursorPos.y))
				scene.Select(*selected);
		}
	}
}
