#pragma once
#include "../interfaces/IGui.h"
#include "../Scene/Scene.h"

class MainMenu : public IGui
{
	Scene& scene;
	Camera& camera;
public:
	MainMenu(Scene& scene, Camera& camera);

	// Inherited via IGui
	virtual bool RenderGui() override;

};