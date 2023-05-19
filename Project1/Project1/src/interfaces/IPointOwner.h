#pragma once
#include <memory>

class Point;
class Scene;
class IPointOwner
{
public:
	virtual void SelectAll(Scene& scene) const = 0;
	virtual bool CanBeDeleted(const Point& p) const = 0;
};