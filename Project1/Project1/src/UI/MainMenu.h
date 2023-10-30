#pragma once
#include "../interfaces/IGui.h"
#include "../Scene/Scene.h"
#include "../Simulations/Simulator.h"
#include "../ErrorDisplayer.h"

class MainMenu : public IGui
{
	Scene& scene;
	Camera& camera;
	Simulator& simulator;
	ErrorDisplayer& errorDisplayer;
public:
	MainMenu(Scene& scene, Camera& camera, Simulator& simulator, ErrorDisplayer& errorDisplayer);

	// Inherited via IGui
	virtual bool RenderGui() override;

};