#include "Center.h"

Center::Center()
	: size(5.0f)
{
	mesh.vertices = {
		0.0f,0.0f,0.0f,
		size,0.0f,0.0f,
		0.0f,size,0.0f,
		0.0f,0.0f,size
	};
	mesh.indices = {
		0,1, 0,2, 0,3
	};
	mesh.Update();
}

void Center::Render(bool selected, VariableManager& vm)
{
	mesh.Render();
}

void Center::UpdateTransform(std::vector<std::pair<std::shared_ptr<ISceneElement>, bool>>& objects)
{
	Transform newTransform;
	int selectedCount = 0;
	for (auto& obj : objects)
		if(obj.second)
		{
			auto objTransformable = std::dynamic_pointer_cast<ITransformable>(obj.first);
			if (!objTransformable)
				continue;
			newTransform.location += objTransformable->getTransform().location;
			selectedCount++;
		}

	newTransform.location /= selectedCount;
	transform.location = newTransform.location;
}
