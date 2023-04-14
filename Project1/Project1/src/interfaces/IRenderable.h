#pragma once
#include "../Rendering/Mesh.h"

class IRenderable
{
public:
	virtual void Render(bool selected) = 0;
};
