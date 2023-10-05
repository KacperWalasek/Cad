#pragma once
#include "../interfaces/IGui.h"
#include "../Scene/Scene.h"
#include "../Simulations/Simulator.h"

class MainMenu : public IGui
{
	Scene& scene;
	Camera& camera;
	Simulator& simulator;
public:
	MainMenu(Scene& scene, Camera& camera, Simulator& simulator);

	// Inherited via IGui
	virtual bool RenderGui() override;

};