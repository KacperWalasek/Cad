#pragma once
#include "../Scene/ISceneElement.h"
#include <memory>

class Scene;
class ISceneTracker
{
public:
	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) = 0;
	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) = 0;
	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) = 0;
	virtual void onMove(Scene& scene, std::shared_ptr<ISceneElement> elem) = 0;
};