#pragma once 
#include "../Rendering/Transform.h"
#include "../Rendering/Mesh.h"
#include "../interfaces/IRenderable.h"
#include "../EventHandlers/EventHanlderInterfaces.h"
#include "../Rendering/Camera.h"

class Cursor : public IRenderable, public IMouseEventHandler
{
	Camera& camera;
	Mesh mesh;
	float size;
public:
	Transform transform;
	Cursor(Camera& camera);

	virtual void Render() override;

	virtual void mouseCallback(GLFWwindow* window, int button, int action, int mods) override;
};