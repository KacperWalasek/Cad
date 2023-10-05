#include "Simulator.h"

void Simulator::update() 
{
	for (auto simulation : simulations)
		if (simulation->isRunning())
			simulation->update();
}

bool Simulator::RenderGui()
{
	for (auto simulation : simulations) 
	{
		auto gui = std::dynamic_pointer_cast<IGui>(simulation);
		gui->RenderGui();
	}
	return false;
}
