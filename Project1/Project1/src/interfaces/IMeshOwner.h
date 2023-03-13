#pragma once
#include "../Rendering/Mesh.h"

class IMeshOwner
{
public:
	virtual Mesh& getMesh() = 0;
};
