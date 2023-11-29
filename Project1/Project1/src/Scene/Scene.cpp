#include "Scene.h"
#include "../interfaces/ITransformable.h"
#include "../interfaces/ISelfControl.h"
#include <algorithm>
#include <imgui/imgui_internal.h>
#include <memory>
#include "../interfaces/IPointOwner.h"
#include "../Geometry/Point.h"

Scene::Scene(std::vector<std::pair<std::shared_ptr<ISceneElement>, bool>> objects, Camera& camera, Simulator& simulator)
	:objects(objects), cursor(std::make_shared<Cursor>(camera)), simulator(simulator)
{
	
}

void Scene::Add(std::shared_ptr<ISceneElement> obj, bool cursorPosition)
{
	if (cursorPosition)
	{
		auto objTransformable = std::dynamic_pointer_cast<ITransformable>(obj);
		if (objTransformable)
			objTransformable->setLocation(cursor->transform.location);
	}
	objects.push_back({ obj,false });
	for (auto& tracker : trackers)
		tracker->onAdd(*this, obj);
	auto objTracker = std::dynamic_pointer_cast<ISceneTracker>(obj);
	if (objTracker)
		trackers.push_back(objTracker);
}

void Scene::Remove(std::shared_ptr<ISceneElement> obj)
{
	auto sc = std::dynamic_pointer_cast<ISelfControl>(obj);
	if (sc && !sc->canBeDeleted())
		return;
	if (lastSelected == obj)
		lastSelected = nullptr;
	for (auto& tracker : trackers)
		tracker->onRemove(*this, obj);
	std::erase_if(objects, 
		[&obj](const std::pair<std::shared_ptr<ISceneElement>,bool>& el) {
			return el.first.get() == obj.get();
		});
}

void Scene::Select(std::pair<std::shared_ptr<ISceneElement>, bool>& obj)
{
	obj.second = true;
	
	lastSelected = obj.first;
	for (auto& tracker : trackers)
		tracker->onSelect(*this, obj.first);

	center.UpdateTransform(objects);
}

void Scene::Select(std::shared_ptr<ISceneElement> obj)
{
	auto it = std::find_if(objects.begin(), objects.end(), 
		[obj](const std::pair<std::shared_ptr<ISceneElement>, bool>& p) {return p.first == obj; });
	Select(*it);
}

bool Scene::RenderGui()
{
	ImGui::Begin("Scene");
	ImGui::Checkbox("Show Points", &showPoints);
	std::vector<const char*> names;
	for (int i = 0 ; i< objects.size(); i++)
		names.push_back(strdup(("##" + std::to_string(i)).data()));

	static int item_current = 0;
	for (int i = 0; i<objects.size(); i++)
	{
		auto p = std::dynamic_pointer_cast<Point>(objects[i].first);
		if (!showPoints && p)
			continue;
		if (ImGui::Selectable(names[i], &objects[i].second))
		{
			bool s = objects[i].second;
			if (!ImGui::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
				for (auto& p : objects)
					if(p!= objects[i])
						p.second = false;
			if (s)
				Select(objects[i]);
		}
		if (ImGui::BeginPopupContextItem()) 
		{
			ImGui::Text(objects[i].first->getName().data());
			
			auto sc = std::dynamic_pointer_cast<ISelfControl>(objects[i].first);
			if (sc && !sc->canBeDeleted())
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::Button("Delete"))
			{
				std::vector<std::shared_ptr<ISceneElement>> selected;
				for (auto el : objects)
				{
					if (el.second)
						selected.push_back(el.first);
				}
				for (auto el : selected) {
					for (int i = 0; i < simulator.simulations.size(); i++)
						if (simulator.simulations[i] == std::dynamic_pointer_cast<ISimulation>(el))
							simulator.simulations.erase(simulator.simulations.begin() + i);
					Remove(el);
				}
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				ImGui::End();
				return false;
			}
			
			if (sc && !sc->canBeDeleted())
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			auto po = std::dynamic_pointer_cast<IOwner>(objects[i].first);
			if (po)
			{
				if (ImGui::Button("Select all points"))
				{
					po->SelectAll(*this);
				}
			}
				
			
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		ImGui::Text(objects[i].first->getName().data());
	}
	ImGui::End();
	return false;
}
