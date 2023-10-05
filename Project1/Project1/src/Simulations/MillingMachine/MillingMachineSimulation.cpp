#include "MillingMachineSimulation.h"
#include "../../UI/FileLoader.h"

void MillingMachineSimulation::start()
{
	running = true;
}

void MillingMachineSimulation::stop()
{
	running = false;
}

void MillingMachineSimulation::reset()
{
	running = true;
}

void MillingMachineSimulation::update()
{
}

bool MillingMachineSimulation::isRunning() const
{
	return running;
}

bool MillingMachineSimulation::RenderGui()
{
	ImGui::Begin("Milling machine simulation");
	if (ImGui::Button("Load Path")) {
		std::string filename = FileLoader::selectFile();
		if (!filename.empty())
		{
			MillingPath path = FileLoader::loadPath(filename);
		}
	}
	ImGui::Text("Material");
	ImGui::InputInt("division x", &divisions.x);
	ImGui::InputInt("division y", &divisions.y);

	ImGui::InputInt("size x", &materialSize.x);
	ImGui::InputInt("size y", &materialSize.y);
	ImGui::InputInt("size z", &materialSize.z);
	ImGui::InputInt("base height", &baseHeight);

	ImGui::Text("Simulation");
	ImGui::InputInt("speed", &speed);
	ImGui::Button("Start");
	ImGui::Button("Instant");

	ImGui::Text("Cutter");
	ImGui::InputInt("size", &cutterSize);
	ImGui::InputInt("height", &height);
	ImGui::Checkbox("flat", &flat);

	ImGui::End();
	return false;
}

bool MillingMachineSimulation::loadPathFile()
{
	std::string filename = FileLoader::selectFile();
	if (!filename.empty())
	{
	}
	return false;
}