#pragma once
#include "EventHanlderInterfaces.h"
#include "../Scene/Scene.h"

class RectSelection : public IMouseEventHandler
{
	Scene& scene;
	Camera& camera;
	glm::dvec2 initialPos;
public:
	RectSelection(Scene& scene, Camera& camera);

	virtual void mouseCallback(GLFWwindow* window, int button, int action, int mods) override;
};