#pragma once
#include "../interfaces/IGui.h"
#include "../Scene/Scene.h"

class MainMenu : public IGui
{
	Scene& scene;
public:
	MainMenu(Scene& scene);

	// Inherited via IGui
	virtual void RenderGui() override;

};