#include "Scene.h"
#include <algorithm>
#include "../interfaces/ITransformable.h"
Scene::Scene(std::vector<std::pair<std::shared_ptr<ISceneElement>, bool>> objects, Camera& camera)
	:objects(objects), cursor(std::make_shared<Cursor>(camera))
{
	
}

void Scene::Add(std::shared_ptr<ISceneElement> obj)
{
	auto objTransformable = std::dynamic_pointer_cast<ITransformable>(obj);
	if(objTransformable)
		objTransformable->getTransform().location = cursor->transform.location;
	objects.push_back({ obj,false });
	for (auto& tracker : trackers)
		tracker->onAdd(*this, obj);
	auto objTracker = std::dynamic_pointer_cast<ISceneTracker>(obj);
	if (objTracker)
		trackers.push_back(objTracker);
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

bool Scene::RenderGui(std::vector<std::shared_ptr<ISceneTracker>>& trackers)
{
	ImGui::Begin("Scene");
	std::vector<const char*> names;
	for (int i = 0 ; i< objects.size(); i++)
		names.push_back(strdup(("##" + std::to_string(i)).data()));

	static int item_current = 0;
	for (int i = 0; i<objects.size(); i++)
	{
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
		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			ImGui::Text(objects[i].first->getName().data());
			if (ImGui::Button("Delete"))
			{
				if (lastSelected == objects[i].first)
					lastSelected = nullptr;
				for (auto& tracker : trackers)
					tracker->onRemove(*this, objects[i].first);
				objects.erase(objects.begin() + i);
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				ImGui::End();
				return false;
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		ImGui::Text(objects[i].first->getName().data());
	}
	ImGui::End();
	return false;
}
