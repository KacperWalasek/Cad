#pragma once
#include "../Rendering/Transform.h"

class ITransformable
{
public:
	virtual const Transform& getTransform() const = 0;
	virtual const void setTransform(const Transform& transform) = 0;
	virtual const void setLocation(const glm::fvec3& location) = 0;
	virtual const void setRotation(const glm::fvec3& rotation) = 0;
	virtual const void setScale(const glm::fvec3& scale) = 0;
};