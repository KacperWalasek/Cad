#pragma once
#include "../Rendering/Transform.h"

class ITransformable
{
public:
	virtual Transform& getTransform() = 0;
};