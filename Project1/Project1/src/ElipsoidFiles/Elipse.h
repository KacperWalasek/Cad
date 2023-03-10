#pragma once
#include <GL/glew.h>
#include "../Math/Matrix4x4.h"
#include "../Rendering/Transform.h"
#include "../Rendering/Window.h"
#include <future>
#include "RGB.h"


class Elipse
{
	Transform transform;
	float a, b, c;
	CadMath::Matrix4x4 D;
	CadMath::Matrix4x4 M;
	CadMath::Matrix4x4 tD;

	std::tuple<bool,float,float> calculateZ(float x, float y) const;
	CadMath::Vector4 calculateNormal(float x, float y, float z) const;
public:
	int currentSizeX, currentSizeY;
	Transform temporaryTransformation;
	float rX, rY, rZ;
	float m;

	Elipse();
	void setRadious(float x, float y, float z);
	RGB CalculatePixelColor(float x, float y) const;

	void updateTransforamtions();
	void applyTemporaryTransformation();

	void Update(Window& window);
};