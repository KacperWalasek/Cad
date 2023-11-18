#include "MillingMachineSimulation.h"
#include "../../UI/FileLoader.h"

std::tuple<int, int, glm::fvec3> MillingMachineSimulation::getPosition(float t)
{ 
	for (int i = 0; i < path.dists.size()-1; i++)
	{
		if (path.dists[i] > t)
		{
			float distDiff = path.dists[i] - path.dists[i - 1];
			float wsp = (t - path.dists[i - 1]) / distDiff;
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
	int from = hms[selectedHM].second.lastVisited;
	hms[selectedHM].second.Finalize();
	errorHandler->Validate(hms[selectedHM].second, materialSize, from, hms[selectedHM].second.GetPath().positions.size());
	errorHandler->UpdateLastStable(materialSize);
	finished = true;
}

MillingMachineSimulation::MillingMachineSimulation()
	: renderer(std::make_shared<TextureRenderer>(divisions[0], divisions[1], 1, true)),
	errorHandler(std::make_shared<MillingErrorHandler>(1200,1200,15,4, hms))
{
	cutter.setPosition({0,0,0});
	materialCube.setSize(materialSize.x, materialSize.y, materialSize.z);
}

void MillingMachineSimulation::start()
{
	running = true;
	renderer = std::make_shared<TextureRenderer>(divisions[0], divisions[1], 1, true);
	dissableInputs = true;
}

void MillingMachineSimulation::stop()
{
	running = false;
}

void MillingMachineSimulation::reset()
{
	passedPathLength = 0;
	running = true;
}

void MillingMachineSimulation::update(float dt)
{
	if (hms.empty())
		return;
	if (instant) {
		renderInstant(); 
		running = false;
		instant = false; 
		return;
	}

	passedPathLength += dt * speed;
	int last = hms[selectedHM].second.lastVisited;
	glm::fvec3 currentPosition = hms[selectedHM].second.SetDistance(passedPathLength);
	cutter.setPosition(currentPosition);
	
	if (last != hms[selectedHM].second.lastVisited)
	{
		errorHandler->Validate(hms[selectedHM].second, materialSize, last, hms[selectedHM].second.lastVisited);
		errorHandler->UpdateLastStable(materialSize);
	}
}

bool MillingMachineSimulation::isRunning() const
{
	return running;
}

std::shared_ptr<MillingErrorHandler> MillingMachineSimulation::GetErrorHandler()
{
	return errorHandler;
}

bool MillingMachineSimulation::RenderGui()
{
	ImGui::Begin("Milling machine simulation");
	if (ImGui::Button("Load Path")) {
		std::string filename = FileLoader::selectFile();
		if (!filename.empty())
		{
			path = FileLoader::loadPath(filename);
			if (hms.size() == 0)
				millingPathVisualizer.setMillingPath(path);
			hms.push_back({ filename, {path, materialSize } });
		}
	}

	if (dissableInputs)
		ImGui::BeginDisabled();
	ImGui::Text("Material");
	if (ImGui::InputInt("division x", &divisions.x) || ImGui::InputInt("division y", &divisions.y))
	{
		materialCube.setDivision(divisions.x, divisions.y);
		errorHandler->SetTextureSize(divisions.x, divisions.y);
	}

	if (ImGui::InputInt("cube width", &materialSize.x))
	{
		materialSize.z = materialSize.x;
		materialCube.setSize(materialSize.x, materialSize.y, materialSize.z);
		for (auto& hm : hms)
			hm.second.SetMaterialSize(materialSize);
	}
	if (ImGui::InputInt("cube height", &materialSize.y))
	{
		materialCube.setSize(materialSize.x, materialSize.y, materialSize.z);
		for (auto& hm : hms)
			hm.second.SetMaterialSize(materialSize);
	}

	if (ImGui::InputInt("base height", &baseHeight))
		errorHandler->SetBaseHeight(baseHeight);

	if (dissableInputs)
		ImGui::EndDisabled();

	ImGui::Text("Simulation");
	ImGui::InputFloat("speed", &speed);
	if (ImGui::Button("Start"))
		start();
	if (ImGui::Button("Instant"))
		instant = true;
	ImGui::Checkbox("Show paths", &showPaths);

	ImGui::Text("Cutter");
	if (ImGui::InputInt("height", &height))
		errorHandler->SetCutterHeight(height);

	if (!hms.empty())
	{
		ImGui::BeginDisabled();
		bool flat = hms[selectedHM].second.IsFlat();
		float radius = hms[selectedHM].second.GetRadius();
		ImGui::Checkbox("flat", &flat);
		ImGui::InputFloat("size", &radius);
		ImGui::EndDisabled();
	}

	ImGui::Text("Paths:");
	static int item_current = 0;
	for (int i = 0; i < hms.size(); i++)
	{
		bool selected = selectedHM == i;
		std::string p = hms[i].first;
		std::string name = p.substr(p.find_last_of("\\") + 1).c_str();

		PathState state = hms[i].second.GetState();
		switch (state)
		{
		case PathState::Off:
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
			break;
		case PathState::Running:
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
			break;
		case PathState::Finished:
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			break;
		}

		if (ImGui::Selectable(name.c_str(), selected))
		{
			if (selectedHM != i) 
			{
				millingPathVisualizer.setMillingPath(hms[i].second.GetPath());
				selectedHM = i;
			}
		}
		ImGui::PopStyleColor();
	}
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
	for(auto& hm : hms)
		renderer->Render(hm.second, vm);
	materialCube.setTexture(renderer->getTextureId());
	cutter.Render(selected, vm);
	materialCube.Render(selected, vm);
	if(showPaths)
		millingPathVisualizer.Render(selected, vm);
}
