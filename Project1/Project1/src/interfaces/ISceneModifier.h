#pragma once
#include <vector>
#include <memory>
#include "ISceneElement.h"

class ISceneModifier
{
public:
	virtual std::vector<std::shared_ptr<ISceneElement>> GetAddedObjects() = 0;
	virtual std::vector<std::shared_ptr<ISceneElement>> GetRemovedObjects() = 0;
};