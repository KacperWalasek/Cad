#pragma once
#include <memory>
#include "Rendering/Window.h"
#include "Rendering/Renderer.h"
#include "EventHandlers/CameraMovement.h"
#include "EventHandlers/SelectedMovement.h"
#include "Rendering/AnaglyphRenderer.h"

class CadApp
{
	Window window;
	Renderer renderer;

	std::shared_ptr<Scene> scene;
	std::shared_ptr<Camera> camera;
	std::vector<std::shared_ptr<IGui>> guis;

	std::shared_ptr<CameraMovement> cameraMovement;
	std::shared_ptr<SelectedMovement> selectedMovement;

public:
	CadApp();
	void Run();
};