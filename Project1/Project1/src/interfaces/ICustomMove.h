#pragma once
#include <glm/glm.hpp>

class ICustomMove
{
public:
	virtual void StartMove() = 0;
	virtual bool Translate(glm::fvec4 translation) = 0;
	virtual bool Rotate(glm::fvec4 rotation) = 0;
	virtual bool Scale(glm::fvec4 scale) = 0;
};