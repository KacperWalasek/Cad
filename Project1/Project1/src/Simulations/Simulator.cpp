#include "Simulator.h"
#include <GLFW/glfw3.h>

void Simulator::update() 
{
	float currentTime = glfwGetTime();
	float dt = currentTime - time;
	time = currentTime;
	for (auto simulation : simulations)
		if (simulation->isRunning())
			simulation->update(dt);
}

bool Simulator::RenderGui()
{
	time = glfwGetTime();
	for (auto simulation : simulations) 
	{
		auto gui = std::dynamic_pointer_cast<IGui>(simulation);
		gui->RenderGui();
	}
	return false;
}
