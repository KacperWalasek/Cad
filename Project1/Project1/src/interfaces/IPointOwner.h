#pragma once
#include <memory>

class Scene;
class IOwner
{
public:
	virtual void SelectAll(Scene& scene) const = 0;
	virtual bool CanChildBeDeleted() const = 0;
};