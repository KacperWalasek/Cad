#pragma once
#include "../Rendering/Mesh.h"
#include "../Rendering/VariableManager.h"

class IRenderable
{
public:
	virtual void Render(bool selected, VariableManager& vm) = 0;
};
