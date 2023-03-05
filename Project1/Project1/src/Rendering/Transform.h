#pragma once
#include "../Math/Matrix4x4.h"

struct Transform
{ 
	Transform()
		: location({ 0,0,0 }), rotation({ 0,0,0 }), scale({ 1,1,1 })
	{}
	Transform(CadMath::Vector4 location, CadMath::Vector4 rotation, CadMath::Vector4 scale)
		: location(location), rotation(rotation), scale(scale)
	{}
	CadMath::Vector4 location;
	CadMath::Vector4 rotation; 
	CadMath::Vector4 scale;

	CadMath::Matrix4x4 GetMatrix() const;
	CadMath::Matrix4x4 GetRotationMatrix() const;
	CadMath::Matrix4x4 GetLocationMatrix() const;
	CadMath::Matrix4x4 GetScaleMatrix() const;
	
	Transform operator+ (const Transform& t);
};