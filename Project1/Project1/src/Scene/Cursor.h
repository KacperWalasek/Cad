#pragma once 
#include "../Rendering/Transform.h"
#include "../Rendering/Mesh.h"
#include "../interfaces/IRenderable.h"
#include "../EventHandlers/EventHanlderInterfaces.h"
#include "../Rendering/Camera.h"

class Cursor : public IRenderable, public IMouseEventHandler, public IGui
{
	Camera& camera;
	Mesh mesh;
	float size;
public:
	Transform transform;
	glm::fvec2 screenPosition;
	Cursor(Camera& camera);

	virtual void Render(bool selected, VariableManager& vm) override;

	virtual void mouseCallback(GLFWwindow* window, int button, int action, int mods) override;

	void UpdateFromScreen();
	void UpdateFromWorld();
	void Update(GLFWwindow* window);

	virtual void RenderGui() override;
};