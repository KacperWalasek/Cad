#include "SelectedMovement.h"
#include "../Rotator.h"
#include "../interfaces/ITransformable.h"
#include "../interfaces/ICustomMove.h"
#include "../interfaces/ISelfControl.h"

SelectedMovement::SelectedMovement(Scene& scene, Camera& camera)
	:scene(scene), camera(camera), mode(None)
{}
void SelectedMovement::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_X:
			if (mode != None)
			{
				xOnly = !xOnly;
				yOnly = false;
				zOnly = false;
			}
			break;
		case GLFW_KEY_Y:
			if (mode != None)
			{
				xOnly = false;
				yOnly = !yOnly;
				zOnly = false;
			}
			break;
		case GLFW_KEY_Z:
			if (mode != None)
			{
				xOnly = false;
				yOnly = false;
				zOnly = !zOnly;
			}
			break;
		case GLFW_KEY_Q:
			if (mode != None)
			{
				for (int i = 0; i < stableTransforms.size(); i++)
				{
					auto objTransformable = std::dynamic_pointer_cast<ITransformable>(stableTransforms[i].first);
					objTransformable->setTransform(stableTransforms[i].second);
				}
				mode = None;
			}
			break;
		}
		if (mode != None)
			return;
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

		if (key == GLFW_KEY_G || key == GLFW_KEY_R || key == GLFW_KEY_S)
		{
			xOnly = false;
			yOnly = false;
			zOnly = false;
		}
		
		glm::dvec2 initPosDouble;
		glfwGetCursorPos(window, &initPosDouble.x, &initPosDouble.y);
		initialPosition = initPosDouble;

		stableTransforms.clear();
		for (auto& el : scene.objects)
			if(el.second)
			{
				auto objSelfControl = std::dynamic_pointer_cast<ISelfControl>(el.first);
				if (objSelfControl && !objSelfControl->canBeMoved())
					continue;

				auto objTransformable = std::dynamic_pointer_cast<ITransformable>(el.first);
				if (objTransformable)
					stableTransforms.push_back({ el.first,objTransformable->getTransform() });

				auto objCustomMovement = std::dynamic_pointer_cast<ICustomMove>(el.first);
				if (objCustomMovement)
					objCustomMovement->StartMove();
				
			}
	}
	if (action == GLFW_RELEASE)
	{
		if ((mode == Translation && key == GLFW_KEY_G) ||
			(mode == Rotation && key == GLFW_KEY_R) ||
			(mode == Scale && key == GLFW_KEY_S))
		{
			mode = None;
			scene.center.UpdateTransform(scene.objects);

			for (auto& tracker : scene.trackers)
				for (auto& el : scene.objects)
					if (el.second)
						tracker->onMove(scene, el.first);
		}
	}
}
void SelectedMovement::Update(GLFWwindow* window)
{
	if (mode == None)
		return;

	int sizeX, sizeY;
	glfwGetWindowSize(window, &sizeX, &sizeY);

	glm::dvec2 currentPos;
	glfwGetCursorPos(window, &currentPos.x, &currentPos.y);
	glm::fvec2 mouseMoveVector = (glm::fvec2)currentPos - initialPosition;

	glm::fvec2 initPosProj(initialPosition.x / (sizeX / 2) - 1.0f, -initialPosition.y / (sizeY / 2) + 1.0f);
	glm::fvec2 currentPosProj(currentPos.x / (sizeX / 2) - 1.0f, -currentPos.y / (sizeY / 2) + 1.0f);

	glm::fvec4 center4 = camera.GetProjectionMatrix() * camera.GetViewMatrix() * scene.center.transform.GetMatrix() * glm::fvec4(0, 0, 0, 1);
	center4 /= center4.w;
	glm::fvec2 center = glm::vec2(center4.x, center4.y);

	bool forceFinish = false;
	for (auto& el : scene.objects)
		if (el.second && mode == SelectedMovement::Translation)
		{
			auto objCustomMove = std::dynamic_pointer_cast<ICustomMove>(el.first);
			if (objCustomMove)
				forceFinish = forceFinish || objCustomMove->Translate(camera.transform.GetMatrix() * glm::fvec4(mouseMoveVector.x / 10, -mouseMoveVector.y / 10, 0.0f, 0.0f));
		}
	if (forceFinish)
		return;
	for (int i = 0; i < stableTransforms.size(); i++)
	{

		auto objTransformable = std::dynamic_pointer_cast<ITransformable>(stableTransforms[i].first);

		Transform& stableTransform = stableTransforms[i].second;

		switch (mode)
		{
		case SelectedMovement::None:
			break;
		case SelectedMovement::Translation:
			auto translateVec = camera.transform.GetMatrix() * glm::fvec4(mouseMoveVector.x / 10, -mouseMoveVector.y / 10, 0.0f, 0.0f);
			if (xOnly)
			{
				translateVec.y = 0;
				translateVec.z = 0;
			}
			if (yOnly)
			{
				translateVec.x = 0;
				translateVec.z = 0;
			}
			if (zOnly)
			{
				translateVec.x = 0;
				translateVec.y = 0;
			}
			objTransformable->setLocation(stableTransform.location + translateVec);
			scene.center.UpdateTransform(scene.objects);
			break;
		case SelectedMovement::Rotation:
		{
			// Rotation
			glm::fvec2 initVec = normalize(initPosProj - center);
			glm::fvec2 curVec = normalize(currentPosProj - center);
			float angle = atan2(curVec.y, curVec.x) - atan2(initVec.y, initVec.x);

			glm::fvec4 rotationAxis = camera.transform.GetMatrix() * glm::fvec4(0, 0, 1, 0);

			glm::fmat4x4 rotationMatrix = Rotator::GetRotationMatrix(rotationAxis, angle) * stableTransform.GetRotationMatrix();
			objTransformable->setRotation(Rotator::MatrixToEuler(rotationMatrix));
			
			// Translation
			glm::fvec4 center4 = camera.GetProjectionMatrix() * camera.GetViewMatrix() * stableTransform.GetMatrix() * glm::fvec4(0, 0, 0, 1);
			center4 /= center4.w;

			glm::fvec4 selectionCenterView = camera.GetInverseProjectionMatrix() * glm::fvec4(center.x,center.y, center4.z,1);
			glm::fvec4 modelCenterView = camera.GetInverseProjectionMatrix() * center4;
			modelCenterView /= modelCenterView.w;
			selectionCenterView /= selectionCenterView.w;

			float currAngle = atan2(modelCenterView.y - selectionCenterView.y, modelCenterView.x - selectionCenterView.x);

			float odl = sqrt(pow(modelCenterView.x - selectionCenterView.x,2) + pow(modelCenterView.y - selectionCenterView.y, 2));
			glm::fvec4 newPointView = selectionCenterView + odl * glm::vec4(cosf(currAngle + angle), sinf(currAngle + angle),0,0);
			glm::fvec4 newPointWorld = camera.transform.GetMatrix() * newPointView;
			newPointWorld /= newPointWorld.w;
			objTransformable->setLocation(newPointWorld - glm::fvec4(0, 0, 0, 1));
			break;
		}
		case SelectedMovement::Scale:
		{
			float odlInit = sqrt(pow(initPosProj.x - center.x, 2) + pow(initPosProj.y - center.y, 2));
			float odl = sqrt(pow(currentPosProj.x - center.x, 2) + pow(currentPosProj.y - center.y, 2));
			float scale = odl/odlInit;
			objTransformable->setScale(stableTransform.scale * glm::fvec4(scale, scale, scale, 0));

			// Translation
			glm::fvec4 modelCenter = camera.GetProjectionMatrix() * camera.GetViewMatrix() * stableTransform.GetMatrix() * glm::fvec4(0, 0, 0, 1);
			modelCenter /= modelCenter.w;

			glm::fvec4 selectionCenterView = camera.GetInverseProjectionMatrix() * glm::fvec4(center.x, center.y, modelCenter.z, 1);
			glm::fvec4 modelCenterView = camera.GetViewMatrix() * stableTransform.GetMatrix() * glm::fvec4(0, 0, 0, 1);
			selectionCenterView /= selectionCenterView.w;
			modelCenterView /= modelCenterView.w;

			glm::fvec4 newPointView = selectionCenterView + scale * (modelCenterView - selectionCenterView);
			glm::fvec4 newPointWorld = camera.transform.GetMatrix() * newPointView;
			newPointWorld /= newPointWorld.w;

			const Transform& selectedTransform = objTransformable->getTransform();
			if (xOnly)
			{
				newPointWorld.y = selectedTransform.location.y;
				newPointWorld.z = selectedTransform.location.z;
			}
			if (yOnly)
			{
				newPointWorld.x = selectedTransform.location.x;
				newPointWorld.z = selectedTransform.location.z;
			}
			if (zOnly)
			{
				newPointWorld.x = selectedTransform.location.x;
				newPointWorld.y = selectedTransform.location.y;
			}
			objTransformable->setLocation(newPointWorld - glm::fvec4(0, 0, 0, 1));

			break;
		}
		default:
			break;
		}
		for (auto& tracker : scene.trackers)
			for (auto& el : scene.objects)
				if (el.second)
					tracker->onMove(scene, el.first);
	}
}
