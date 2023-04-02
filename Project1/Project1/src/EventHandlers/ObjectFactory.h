#pragma once
#include "EventHanlderInterfaces.h"
#include "../Scene/Scene.h"

class ObjectFactory : public IKeyEventHandler
{
	Scene& scene;
public:
	ObjectFactory(Scene& scene);

	virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
};