#include "Collapse.h"
#include "../Geometry/Point.h"
#include <algorithm>
#include <memory>
#include <set>

Collapse::Collapse(Scene& scene)
	:scene(scene)
{
}

void Collapse::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{	
		std::vector<std::shared_ptr<Point>> pointList;
		std::vector<int> indList;
		glm::fvec4 position = {0,0,0,0};
		std::list<std::weak_ptr<IOwner>> owners;
		for (int i = 0; i < scene.objects.size(); i++)
		{
			if (!scene.objects[i].second)
				continue;
			auto p = std::dynamic_pointer_cast<Point>(scene.objects[i].first);
			if (!p)
				continue;
		
			for (auto& o : p->po)
			{
				auto ownerAddress = o.lock().get();
				if (std::find_if(owners.begin(), owners.end(), [&ownerAddress](const std::weak_ptr<IOwner>& owner) {
					return owner.lock().get() == ownerAddress;
					}) == owners.end())
					owners.push_back(o);
			}
			pointList.push_back(p);
			indList.push_back(i);
			position += p->getTransform().location;
		}
		
		position /= (float)pointList.size();
		auto newP = std::make_shared<Point>(position);
		newP->po = owners;

		for (auto& t : scene.trackers)
			t->onCollapse(scene, pointList, newP);

		std::sort(indList.rbegin(), indList.rend());
		for (auto& i : indList)
			scene.objects.erase(scene.objects.begin() + i);

		scene.objects.push_back({ newP,true });
		scene.lastSelected = newP;
	}
}
