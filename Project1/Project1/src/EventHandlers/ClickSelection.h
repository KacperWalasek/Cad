#pragma once
#include "EventHanlderInterfaces.h"
#include "../Scene/Scene.h"

class ClickSelection : public IMouseEventHandler
{
	Scene& scene;
	Camera& camera;
public:
	ClickSelection(Scene& scene, Camera& camera);

	virtual void mouseCallback(GLFWwindow* window, int button, int action, int mods) override;
};