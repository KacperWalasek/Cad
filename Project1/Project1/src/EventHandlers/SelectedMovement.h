#pragma once
#include "../Rendering/Camera.h"
#include "EventHanlderInterfaces.h"
#include "../Scene/Scene.h"

class SelectedMovement : public IKeyEventHandler
{
	enum Transformation {
		None, Translation, Rotation, Scale
	};
	Transformation mode;
	glm::fvec2 initialPosition;
	Scene& scene;
	Camera& camera;
	std::vector<std::pair<std::shared_ptr<ISceneElement>, Transform>> stableTransforms;
	Transform temporaryTransform;
public:
	SelectedMovement(Scene& scene, Camera& camera);
	virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void Update(GLFWwindow* window);
};