#include "SurfaceCreator.h"

Indexer SurfaceCreator::indexer;

void SurfaceCreator::updateSurface()
{
	surface->sizeX = sizeX;
	surface->sizeY = sizeY;
	surface->cylinder = cylinder;
	surface->countX = surfaceCount[0];
	surface->countY = surfaceCount[1];

	surface->Recalculate();
}

void SurfaceCreator::apply()
{
	auto it = std::find_if(scene.objects.begin(), scene.objects.end(),
		[this](std::pair<std::shared_ptr<ISceneElement>, bool> elem) {
			return elem.first.get() == this;
		});
	
	toRemove = it->first;
	toAdd.push_back(surface);
	for (auto& p : surface->points)
	{
		p->po = surface;
		toAdd.push_back(p);
	}
}

SurfaceCreator::SurfaceCreator(Scene& scene)
	: scene(scene), name("SurfaceCreator-" + std::to_string(indexer.getNewIndex())),
	toRemove(nullptr),
	sizeX(10),
	sizeY(10),
	cylinder(false)
{
	surfaceCount[0] = 1;
	surfaceCount[1] = 1;
	surface = std::make_shared<SurfaceC0>(scene.cursor->transform.location, surfaceCount[0], surfaceCount[1], sizeX, sizeY, cylinder);
}

bool SurfaceCreator::RenderGui()
{
	ImGui::Begin("Surface creator");
	if (ImGui::Checkbox("Cylindric", &cylinder))
	{
		if(cylinder && surfaceCount[0]<3)
			surfaceCount[0] = 4;
		updateSurface();
	}
	if (ImGui::InputInt2("Point count", surfaceCount))
	{
		surfaceCount[0] = glm::max(surfaceCount[0], cylinder ? 3 : 1);
		surfaceCount[1] = glm::max(surfaceCount[1], 1);
		updateSurface();
	}
	if (ImGui::InputFloat(cylinder ? "Radius" : "Lenght", &sizeX))
	{
		if (sizeX <= 0)
			sizeX = 1;
		updateSurface();
	}
	if(ImGui::InputFloat("Height", &sizeY))
	{
		if (sizeY <= 0)
			sizeY = 1;
		updateSurface();
	}
	
	if (ImGui::Button("Apply"))
		apply();
	
	ImGui::End();
	return false;
}

std::vector<std::shared_ptr<ISceneElement>> SurfaceCreator::GetAddedObjects()
{
	return toAdd;
}

std::vector<std::shared_ptr<ISceneElement>> SurfaceCreator::GetRemovedObjects()
{
	if (toRemove)
		return { toRemove };
	return {};
}

void SurfaceCreator::Render(bool selected, VariableManager& vm)
{
	surface->Render(selected, vm);
}

std::string SurfaceCreator::getName() const
{
	return name;
}
