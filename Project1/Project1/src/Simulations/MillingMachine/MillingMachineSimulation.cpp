#include "MillingMachineSimulation.h"
#include "../../UI/FileLoader.h"

glm::fvec3 MillingMachineSimulation::getPosition(float t)
{
	for (int i = 0; i < path.times.size()-1; i++)
	{
		if (path.times[i] > t)
		{
			float timeDiff = path.times[i] - path.times[i - 1];
			float wsp = (t - path.times[i - 1]) / timeDiff;
			return wsp * path.positions[i] + (1-wsp) * path.positions[i-1];
		}
	}
	return path.positions[path.positions.size() - 1];
}

void MillingMachineSimulation::createTexture()
{
	for(int i =0; i<divisions[0]; i++)
		for (int j = 0; j < divisions[1]; j++)
		{
				tex.setPixel(i, j, { GLbyte(0),GLbyte(255),GLbyte(0) });
		}
	glm::fvec3 last = { 0,0,0 };
	for (int i = 1; i < path.positions.size(); i++)
	{
		glm::fvec3 pos = ((path.positions[i] + 100.0f) / 200.0f) * glm::fvec3(divisions[0],divisions[1],1.0f);
		applyStep(last, pos);
		last = pos;
	}

	tex.flush();
}

void MillingMachineSimulation::applyStep(glm::fvec3 p1, glm::fvec3 p2)
{
	glm::fvec3 diff = p2 - p1;
	float stepCount = glm::max(glm::abs(diff.x), glm::abs(diff.y));
	for (int j = 0; j < stepCount; j++)
	{
		float mult = j / stepCount;

		tex.setPixel(p1.x + mult * diff.x, p1.y + mult * diff.y, { GLbyte(255 * (p1.z + mult * diff.z)),0,0 });
	}
}

void MillingMachineSimulation::start()
{
	running = true;
	createTexture();
	materialCube.setTexture(tex);
}

void MillingMachineSimulation::stop()
{
	running = false;
}

void MillingMachineSimulation::reset()
{
	timePassed = 0;
	running = true;
}

void MillingMachineSimulation::update(float dt)
{
	timePassed += dt;
	float timeNormalized = timePassed / speed;
	position = getPosition(timeNormalized);
	cutter.setPosition(position);
	
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
			path = FileLoader::loadPath(filename);
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
	if (ImGui::Button("Start"))
		start();
	ImGui::Button("Instant");

	ImGui::Text("Cutter");
	ImGui::InputInt("size", &cutterSize);
	ImGui::InputInt("height", &height);
	ImGui::Checkbox("flat", &flat);

	ImGui::End();
	return false;
}

std::string MillingMachineSimulation::getName() const
{
	return "Milling simulation";
}

void MillingMachineSimulation::Render(bool selected, VariableManager& vm)
{
	cutter.Render(selected, vm);
	materialCube.Render(selected, vm);
}
