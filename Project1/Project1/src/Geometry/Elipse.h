#pragma once
#include <GL/glew.h>
#include "../Math/Matrix4x4.h"
#include "../Rendering/Transform.h"
#define TexSizeX 160
#define TexSizeY 90

class Elipse
{
	Transform transform;
	float a, b, c;
	CadMath::Matrix4x4 D;
public:
	GLubyte image[TexSizeY][TexSizeX][4];
	Elipse();
	void setRadious(float x, float y, float z);
	unsigned int createTexture();
};