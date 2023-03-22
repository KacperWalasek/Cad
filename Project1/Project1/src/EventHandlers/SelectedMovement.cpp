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
		stableTransforms.clear();
		for (auto& el : scene.objects)
			stableTransforms.push_back(el->getTransform());
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

	int sizeX, sizeY;
	glfwGetWindowSize(window, &sizeX, &sizeY);
	glm::fvec2 initPosProj(initialPosition.x / (sizeX / 2) - 1.0f, -initialPosition.y / (sizeY / 2) + 1.0f);
	glm::fvec2 currentPosProj(currentPos.x / (sizeX / 2) - 1.0f, -currentPos.y / (sizeY / 2) + 1.0f);

	glm::fvec2 center = { 0,0 };
	for (int i = 0; i < stableTransforms.size(); i++)
	{
		glm::fvec4 center4 = camera.GetProjectionMatrix() * camera.GetViewMatrix() * stableTransforms[i].GetMatrix() * glm::fvec4(0, 0, 0, 1);
		center4 /= center4.w;
		center += glm::vec2( center4.x, center4.y );
	}
	center /= stableTransforms.size();

	for (int i = 0; i < stableTransforms.size(); i++)
	{
		Transform& selectedTransform = scene.objects[i]->getTransform();
		Transform& stableTransform = stableTransforms[i];

		switch (mode)
		{
		case SelectedMovement::None:
			break;
		case SelectedMovement::Translation:
			selectedTransform.location = stableTransform.location + camera.transform.GetMatrix() * glm::fvec4(mouseMoveVector.x / 10, -mouseMoveVector.y / 10, 0.0f, 0.0f);
			break;
		case SelectedMovement::Rotation:
		{
			glm::fvec2 initVec = normalize(initPosProj - center);
			glm::fvec2 curVec = normalize(currentPosProj - center);
			float angle = atan2(curVec.y, curVec.x) - atan2(initVec.y, initVec.x);

			glm::fvec4 zView = camera.transform.GetMatrix() * glm::fvec4(0, 0, 1, 0);

			glm::fmat4x4 rotationMatrix = Rotator::GetRotationMatrix(zView, angle) * stableTransform.GetRotationMatrix();
			selectedTransform.rotation = Rotator::MatrixToEuler(rotationMatrix);

			
			glm::fvec4 center4 = camera.GetProjectionMatrix() * camera.GetViewMatrix() * stableTransform.GetMatrix() * glm::fvec4(0, 0, 0, 1);
			center4 /= center4.w;

			float currAngle = atan2(center4.y - center.y, center4.x - center.x);

			glm::fvec4 centerView = camera.GetInverseProjectionMatrix() * glm::fvec4(center.x,center.y, center4.z,1);
			glm::fvec4 center4View = camera.GetInverseProjectionMatrix() * center4;
			centerView /= centerView.w;
			center4View /= center4View.w;

			float odl = sqrt(pow(center4View.x- centerView.x,2) + pow(center4View.y - centerView.y, 2));
			glm::fvec4 newPointView = centerView + odl * glm::vec4(cosf(currAngle + angle), sinf(currAngle + angle),0,0);
			glm::fvec4 newPointWorld = camera.transform.GetMatrix() * newPointView;
			newPointWorld /= newPointWorld.w;
			selectedTransform.location = newPointWorld - glm::fvec4(0,0,0,1);
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
}
