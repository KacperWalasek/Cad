#pragma once
#include "../interfaces/IGui.h"
#include "../Scene/Scene.h"

class MillingGui : public IGui {
	Scene& scene;
public:
	MillingGui(Scene& scene);
	// Inherited via IGui
	bool RenderGui() override;
};