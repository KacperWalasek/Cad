#include "CadApp.h"
#include "UI/MainMenu.h"
#include "EventHandlers/ClickSelection.h"
#include "EventHandlers/RectSelection.h"
#include "EventHandlers/ObjectFactory.h"
#include "EventHandlers/Collapse.h"
#include "EventHandlers/CameraMainPositions.h"

CadApp::CadApp()
	: renderer(window), 
	camera(std::make_shared<Camera>(window, 1, 10, 1000))
{

	camera->transform.location.z = -100;
	window.Init();
	renderer.Init();
	scene = std::make_shared<Scene>(
		std::vector<std::pair<std::shared_ptr<ISceneElement>, bool>>(),
		*camera);
	simulator = std::make_shared<Simulator>();
	auto factory = std::make_shared<ObjectFactory>(*scene);
	
	guis = {
		camera,
		scene,
		simulator,
		std::make_shared<MainMenu>(*scene, *camera, *simulator),
		scene->cursor,
		factory
	};


	cameraMovement = std::make_shared<CameraMovement>(*camera);
	window.mouseCallbacks = {
		cameraMovement,
		scene->cursor,
		std::make_shared<ClickSelection>(*scene,*camera),
		std::make_shared<RectSelection>(*scene,*camera) };

	selectedMovement = std::make_shared<SelectedMovement>(*scene, *camera);
	window.keyCallbacks = {
		selectedMovement,
		factory,
		std::make_shared<Collapse>(*scene),
		std::make_shared<CameraMainPositions>(*camera)
	};
}

void CadApp::Run()
{
	auto anaglyphRenderer = std::make_shared<AnaglyphRenderer>(*camera);
	anaglyphRenderer->Init();
	guis.push_back(anaglyphRenderer);
	while (window.isOpen())
	{
		simulator->update();
		window.Update();

		cameraMovement->Update(window.window);
		selectedMovement->Update(window.window);
		
		scene->cursor->Update(window.window);
		if(anaglyphRenderer->enabled)
			anaglyphRenderer->Render(renderer,*scene,guis,*camera);
		else
		{
			renderer.BeginRender(*camera);
			renderer.RenderScene(*camera,*scene);
			renderer.RenderGui(*scene, guis);
		}
		renderer.Finalize(*scene);

		glfwSwapBuffers(window.window);
		glfwPollEvents();
	}
}
