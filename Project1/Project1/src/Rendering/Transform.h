#pragma once
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "../interfaces/IGui.h"
#include <nlohmann/json.hpp>

const float MIN_SIZE = 0.0000001f;

struct Transform : public IGui
{
	Transform()
		: location(0,0,0,0), rotation(0,0,0,0), scale(1,1,1,0), reversedOrder(false)
	{}
	Transform(glm::fvec4 location, glm::fvec4 rotation, glm::fvec4 scale)
		: location(location), rotation(rotation), scale(scale), reversedOrder(false)
	{}
	Transform(nlohmann::json json);
	bool reversedOrder;
	glm::fvec4 location;
	glm::fvec4 rotation;
	glm::fvec4 scale;

	glm::fmat4x4 GetMatrix() const;
	glm::fmat4x4 GetInverseMatrix() const;
	glm::fmat4x4 GetRotationMatrix() const;
	glm::fmat4x4 GetInverseOrderRotationMatrix() const;
	glm::fmat4x4 GetLocationMatrix() const;
	glm::fmat4x4 GetScaleMatrix() const;
	
	Transform operator+ (const Transform& t);

	virtual bool RenderGui() override;

	nlohmann::json Serialize() const;
};