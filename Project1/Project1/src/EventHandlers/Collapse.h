#pragma once
#include "EventHanlderInterfaces.h"
#include "../Scene/Scene.h"

class Collapse : public IKeyEventHandler
{
	Scene& scene;
public:
	Collapse(Scene& scene);

	virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;

};
