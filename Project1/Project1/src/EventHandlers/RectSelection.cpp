#include "RectSelection.h"
#include "../Geometry/Point.h"

RectSelection::RectSelection(Scene& scene, Camera& camera)
	:scene(scene), camera(camera)
{
}

void RectSelection::mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &initialPos.x, &initialPos.y);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		glm::dvec2 currentPos;
		int sizeX, sizeY;
		glfwGetCursorPos(window, &currentPos.x, &currentPos.y);
		glfwGetWindowSize(window, &sizeX, &sizeY);

		glm::fvec2 initCursorPos(initialPos.x / (sizeX / 2) - 1.0f, -initialPos.y / (sizeY / 2) + 1.0f);
		glm::fvec2 currentCursorPos(currentPos.x / (sizeX / 2) - 1.0f, -currentPos.y / (sizeY / 2) + 1.0f);

		float bestZ = 1;
		std::vector<std::pair<std::shared_ptr<ISceneElement>, bool>> selected;
		for (auto& obj : scene.objects)
		{
			std::shared_ptr<Point> point = std::dynamic_pointer_cast<Point>(obj.first);
			if (!point)
				continue;
			auto pointProjPos = camera.GetProjectionMatrix() * camera.GetViewMatrix() * point->getTransform().GetMatrix() * glm::fvec4( 0, 0, 0, 1 );
			pointProjPos /= pointProjPos.w;

			if ((pointProjPos.x > initCursorPos.x) != (pointProjPos.x > currentCursorPos.x) &&
				(pointProjPos.y > initCursorPos.y) != (pointProjPos.y > currentCursorPos.y))
				selected.push_back(obj);
		}
		if (!selected.empty() && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
			for (auto& obj : scene.objects)
			{
				obj.second = false;
				std::shared_ptr<IClickable> clickable = std::dynamic_pointer_cast<IClickable>(obj.first);
				if (clickable)
					clickable->Unclick();
			}
		for(auto& s : selected)
		{
			scene.Select(s.first);
		}
	}
}
