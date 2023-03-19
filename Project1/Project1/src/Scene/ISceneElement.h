#pragma once
#include <string>
#include "../Rendering/Transform.h"

class ISceneElement
{
public:
	virtual std::string getName() const = 0;
	virtual Transform& getTransform() = 0;
};