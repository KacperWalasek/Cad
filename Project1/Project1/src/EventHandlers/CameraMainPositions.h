#pragma once
#include "EventHanlderInterfaces.h"
#include "../Rendering/Camera.h"

class CameraMainPositions : public IKeyEventHandler
{
	Camera& camera;
public:
	CameraMainPositions(Camera& camera);
	virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;

};