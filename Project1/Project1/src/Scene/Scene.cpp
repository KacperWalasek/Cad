#include "Scene.h"

Scene::Scene(std::vector<std::shared_ptr<ISceneElement>> objects, Camera& camera)
	:objects(objects),selected(objects[0]), cursor(std::make_shared<Cursor>(camera))
{
	
}

void Scene::RenderGui()
{
	ImGui::Begin("Scene");
	std::vector<const char*> names;
	for (auto& o : objects)
		names.push_back(strdup(o->getName().data()));

	static int item_current = 0;
	if (ImGui::ListBox("##", &item_current, &(names[0]), names.size(), names.size()))
		selected = objects[item_current];
	ImGui::End();
}
