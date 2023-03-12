#pragma once 
#include "Transform.h"
class Camera
{
	Transform transform;
public:
	CadMath::Matrix4x4 GetViewMatrix() const;
	
};