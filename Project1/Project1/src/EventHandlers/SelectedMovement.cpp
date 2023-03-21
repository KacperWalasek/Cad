#include "SelectedMovement.h"
#include "../Rotator.h"


SelectedMovement::SelectedMovement(Scene& scene, Camera& camera)
	:scene(scene), camera(camera)
{}
void SelectedMovement::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_G:
			mode = Translation;
			break;
		case GLFW_KEY_R:
			mode = Rotation;
			break;
		case GLFW_KEY_S:
			mode = Scale;
			break;
		default:
			return;
		}
		glm::dvec2 initPosDouble;
		glfwGetCursorPos(window, &initPosDouble.x, &initPosDouble.y);
		initialPosition = initPosDouble;
		stableTransform = scene.selected->getTransform();
	}
	if (action == GLFW_RELEASE)
	{
		if ((mode == Translation && key == GLFW_KEY_G) ||
			(mode == Rotation && key == GLFW_KEY_R) ||
			(mode == Scale && key == GLFW_KEY_S))
			mode = None;
	}
}
void SelectedMovement::Update(GLFWwindow* window)
{
	glm::dvec2 currentPos;
	glfwGetCursorPos(window, &currentPos.x, &currentPos.y);
	glm::fvec2 mouseMoveVector = (glm::fvec2)currentPos - initialPosition;
	
	Transform& selectedTransform = scene.selected->getTransform();
	switch (mode)
	{
	case SelectedMovement::None:
		break;
	case SelectedMovement::Translation:
		selectedTransform.location = stableTransform.location + /*stableTransform.GetInverseMatrix() */ camera.transform.GetMatrix() * glm::fvec4(mouseMoveVector.x / 10, -mouseMoveVector.y / 10, 0.0f, 0.0f);
		break;
	case SelectedMovement::Rotation:
	{
		int sizeX, sizeY;
		glfwGetWindowSize(window, &sizeX, &sizeY);

		glm::fvec2 initPosProj(initialPosition.x / (sizeX / 2) - 1.0f, -initialPosition.y / (sizeY / 2) + 1.0f);
		glm::fvec2 currentPosProj(currentPos.x / (sizeX / 2) - 1.0f, -currentPos.y / (sizeY / 2) + 1.0f);

		glm::fvec4 center4 = camera.GetProjectionMatrix() * camera.GetViewMatrix() * stableTransform.GetMatrix()* glm::fvec4(0, 0, 0, 1);
		glm::fvec2 center(center4.x, center4.y);

		glm::fvec2 initVec = normalize(initPosProj - center);
		glm::fvec2 curVec = normalize(currentPosProj - center);
		float angle = atan2(curVec.y, curVec.x) - atan2(initVec.y, initVec.x);

		glm::fvec4 zView = stableTransform.GetInverseMatrix() * camera.transform.GetMatrix() * glm::fvec4(0, 0, 1, 0);
		
		glm::fmat4x4 rotationMatrix = Rotator::GetRotationMatrix(zView, angle);
		selectedTransform.rotation = Rotator::MatrixToEuler(rotationMatrix);
		//selectedTransform.rotation = stableTransform.rotation + stableTransform.GetInverseMatrix() * camera.transform.GetMatrix() * glm::fvec4(-mouseMoveVector.y / 1000, -mouseMoveVector.x / 1000, 0.0f, 0.0f);
		break;
	}
	case SelectedMovement::Scale:
	{
		float scale = fmax(0, 100 - mouseMoveVector.x) / 100;
		selectedTransform.scale = stableTransform.scale * camera.transform.GetMatrix() * glm::fvec4(scale, scale, scale, 0);
		break;
	}
	default:
		break;
	}
}
