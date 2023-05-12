#pragma once 
#include "Transform.h"
#include <glm/glm.hpp>
#include "Window.h"
#include "../interfaces/IGui.h"

class Camera : public IGui
{
	Window& window;
public:
	float fov;
	float n;
	float f;

	Camera(Window& window, float fov, float n, float f);
	Transform transform;
	glm::fmat4x4 GetViewMatrix() const;
	glm::fmat4x4 GetProjectionMatrix() const;
	glm::fmat4x4 GetInverseProjectionMatrix() const;

	float GetAspect() const;
	
	virtual bool RenderGui(std::vector<std::shared_ptr<ISceneTracker>>& trackers) override;
};