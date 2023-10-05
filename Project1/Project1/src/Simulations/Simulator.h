#pragma once
#include "ISimulation.h"
#include <vector>
#include <memory>
#include "../interfaces/IGui.h"

class Simulator : public IGui {
public:
	std::vector<std::shared_ptr<ISimulation>> simulations;
	void update();

	// Inherited via IGui
	virtual bool RenderGui() override;
};