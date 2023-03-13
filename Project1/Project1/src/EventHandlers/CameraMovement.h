#pragma once
#include "../Rendering/Camera.h"
#include "EventHanlderInterfaces.h"

class CameraMovement : public IMouseEventHandler
{
	Camera& camera;
	Transform stableTransform;
	Transform tmpTransform;

	bool isCrtlPressed = false;
	bool isShiftPressed = false;
	bool shouldApply = false;
	double initialMousePosX = 0;
	double initialMousePosY = 0;
	double curentMouseVectorX = 0;
	double curentMouseVectorY = 0;
	float lastTouchTime = 0;
public:
	CameraMovement(Camera& camera);
	virtual void mouseCallback(GLFWwindow* window, int button, int action, int mods) override;

	void Update(GLFWwindow* window);
};