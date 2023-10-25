#include "MillingMachineSimulation.h"
#include "../../UI/FileLoader.h"

std::tuple<int, int, glm::fvec3> MillingMachineSimulation::getPosition(float t)
{ 
	for (int i = 0; i < path.times.size()-1; i++)
	{
		if (path.times[i] > t)
		{
			float timeDiff = path.times[i] - path.times[i - 1];
			float wsp = (t - path.times[i - 1]) / timeDiff;
			return { i-1,i, wsp * path.positions[i] + (1 - wsp) * path.positions[i - 1] };
		}
	}
	return { path.positions.size() - 2, path.positions.size() - 1, path.positions[path.positions.size() - 1] };
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

void MillingMachineSimulation::renderInstant()
{
	for (int i = lastVisited; i < path.positions.size()-1; i++)
		hm.AddSegment(path.positions[i] * sizeMultiplier,
			path.positions[i + 1] * sizeMultiplier,
			path.radius * sizeMultiplier);

	finished = true;
}

MillingMachineSimulation::MillingMachineSimulation()
	: renderer(std::make_shared<TextureRenderer>(divisions[0], divisions[1], 1, true))
{
	cutter.setPosition({0,0,0});

}

void MillingMachineSimulation::start()
{
	running = true;
	renderer = std::make_shared<TextureRenderer>(divisions[0], divisions[1], 1, true);
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
	if (instant) {
		renderInstant();
		running = false;
		return;
	}

	timePassed += dt;
	float timeNormalized = timePassed / speed/5.0f;
	auto [prevIndex, nextIndex, currentPosition] = getPosition(timeNormalized);
	
	if (lastVisited != prevIndex)
	{
		for (int i = lastVisited; i < prevIndex; i++)
			hm.AddSegment(path.positions[i] * sizeMultiplier,
				path.positions[i + 1] * sizeMultiplier,
				path.radius * sizeMultiplier);
		lastVisited = prevIndex;
	}

	hm.AddTemporarySegment(path.positions[prevIndex] * sizeMultiplier,
		currentPosition * sizeMultiplier,
		path.radius * sizeMultiplier);

	cutter.setPosition(currentPosition);
	
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
			path = FileLoader::loadPath(filename);
			hm.setRadius(path.radius * sizeMultiplier);
			hm.setFlat(path.flat);
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
	if (ImGui::Button("Start"))
		start();
	if (ImGui::Button("Instant"))
		instant = true;

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
	// depth is in r
	renderer->Clear({ 1,0,0,1 });
	renderer->Render(hm, vm);
	materialCube.setTexture(renderer->getTextureId());
	cutter.Render(selected, vm);
	materialCube.Render(selected, vm);
}
