#include "MillingGui.h"
#include "../Geometry/Intersection.h"
#include <memory>
#include "MillingPathCreator.h"
#include "../Debuger/Debuger.h"

MillingGui::MillingGui(Scene& scene)
	: scene(scene)
{}

bool MillingGui::RenderGui()
{
	ImGui::Begin("Milling");
	ImGui::InputFloat("u mult", &MillingPathCreator::uDistMult);
	ImGui::InputFloat("v mult", &MillingPathCreator::vDistMult);
	ImGui::InputFloat("min dist", &MillingPathCreator::minDetailDist);
	ImGui::InputFloat("max dist", &MillingPathCreator::maxDetailDist);
	ImGui::InputFloat("frequency", &MillingPathCreator::frequencyMultipier);
	ImGui::InputFloat("dencity", &MillingPathCreator::dencityMultipier);
	ImGui::InputFloat("frequency curvature influence", &MillingPathCreator::frequecnyCurvatureInfluence);
	ImGui::InputFloat("dencity curvature influence", &MillingPathCreator::dencityCurvatureInfluence);
	if (ImGui::Button("Recalculate"))
	{
		std::vector<std::shared_ptr<ISceneElement>> intersections;
		for (int i = 0; i < scene.objects.size(); i++)
		{
			if (std::dynamic_pointer_cast<Intersection>(scene.objects[i].first))
				intersections.push_back(scene.objects[i].first);
		}
		for (auto i : intersections)
			scene.Remove(i);

		Debuger::Clear();
		auto p = MillingPathCreator::CreateDetailPath(scene);
	}
	ImGui::End();
	return false;
}
