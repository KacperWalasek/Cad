#pragma once
#include "ISceneElement.h"
#include <memory>
#include <vector>

class Scene;
class Point;
class ISceneTracker
{
public:
	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) = 0;
	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) = 0;
	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) = 0;
	virtual void onMove(Scene& scene, std::shared_ptr<ISceneElement> elem) = 0;
	virtual void onCollapse(Scene& scene, std::vector<std::shared_ptr<Point>>& collapsed, std::shared_ptr<Point> result) = 0;
};