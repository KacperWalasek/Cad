#include "SurfaceCreator.h"

Indexer SurfaceCreator::indexer;

void SurfaceCreator::updateSurface()
{
	surfaceC0->sizeX = sizeX;
	surfaceC0->sizeY = sizeY;
	surfaceC0->cylinder = cylinder;
	surfaceC0->countX = surfaceCount[0];
	surfaceC0->countY = surfaceCount[1];

	surfaceC0->Recalculate();
	
	surfaceC2->sizeX = sizeX;
	surfaceC2->sizeY = sizeY;
	surfaceC2->cylinder = cylinder;
	surfaceC2->countX = surfaceCount[0];
	surfaceC2->countY = surfaceCount[1];

	surfaceC2->Recalculate();
}

void SurfaceCreator::apply()
{
	auto it = std::find_if(scene.objects.begin(), scene.objects.end(),
		[this](std::pair<std::shared_ptr<ISceneElement>, bool> elem) {
			return elem.first.get() == this;
		});
	
	toRemove = it->first;
	
	if (c0)
	{
		surfaceC0->CreateControlPoints();
		toAdd.push_back(surfaceC0);
		for (auto& p : surfaceC0->points)
		{
			p->po.push_back(surfaceC0);
			toAdd.push_back(p);
		}
	}
	else
	{
		surfaceC2->CreateControlPoints();
		toAdd.push_back(surfaceC2);
		for (auto& p : surfaceC2->points)
		{
			p->po.push_back(surfaceC2);
			toAdd.push_back(p);
		}
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
	surfaceC0 = std::make_shared<SurfaceC0>(scene.cursor->transform.location, surfaceCount[0], surfaceCount[1], sizeX, sizeY, cylinder);
	surfaceC2 = std::make_shared<SurfaceC2>(scene.cursor->transform.location, surfaceCount[0], surfaceCount[1], sizeX, sizeY, cylinder);

}

bool SurfaceCreator::RenderGui()
{
	ImGui::Begin("Surface creator");
	ImGui::Checkbox("C0", &c0);
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
	if(c0)
		surfaceC0->Render(selected, vm);
	else
		surfaceC2->Render(selected, vm);
}

std::string SurfaceCreator::getName() const
{
	return name;
}
