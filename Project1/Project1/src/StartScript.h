#pragma once
#include "Scene/Scene.h"
#include "ErrorDisplayer.h"

class StartScript {
	const bool dissabled = true;
	Scene& scene;
	Simulator& simulator;
	ErrorDisplayer& errorDisplayer;
public:
	StartScript(Scene& scene, Simulator& simulator, ErrorDisplayer& errorDisplayer);
	void Run();
};