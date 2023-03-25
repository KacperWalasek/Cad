#include "Scene.h"
#include <algorithm>
Scene::Scene(std::vector<std::pair<std::shared_ptr<ISceneElement>, bool>> objects, Camera& camera)
	:objects(objects), cursor(std::make_shared<Cursor>(camera))
{
	
}

void Scene::Add(std::shared_ptr<ISceneElement> obj)
{
	for (auto& tracker : trackers)
		tracker->onAdd(*this, obj);
	obj->getTransform().location = cursor->transform.location;
	objects.push_back({ obj,false });
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
	
}

void Scene::RenderGui()
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
			if (objects[i].second)
				Select(objects[i]);
			{
				lastSelected = objects[i].first;
				for (auto& tracker : trackers)
					tracker->onSelect(*this, objects[i].first);
			}
			if (!ImGui::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
				for (auto& p : objects)
					if(p!= objects[i])
						p.second = false;
			center.UpdateTransform(objects);
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
				return;
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		ImGui::Text(objects[i].first->getName().data());
	}
	ImGui::End();
}
