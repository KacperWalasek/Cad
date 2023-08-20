#pragma once
#include <memory>
#include "ISceneElement.h"

class Scene;
class IOwner
{
public:
	virtual void SelectAll(Scene& scene) const = 0;
	virtual bool CanChildBeDeleted() const = 0;
	virtual bool CanChildBeMoved() const = 0;
	virtual void ChildMoved(ISceneElement& child) = 0;
};