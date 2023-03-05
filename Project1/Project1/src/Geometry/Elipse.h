#pragma once
#include <GL/glew.h>
#include "../Math/Matrix4x4.h"
#include "../Rendering/Transform.h"
#define TexSizeX 1600
#define TexSizeY 900

typedef struct {
	unsigned char
		r,
		g,
		b;
}RGB;

class Elipse
{
	Transform transform;
	float rX, rY, rZ;
	float a, b, c;
	float m; 
	CadMath::Matrix4x4 D;
	CadMath::Matrix4x4 M;
	CadMath::Matrix4x4 tD;
	std::tuple<bool,float,float> calculateZ(float x, float y) const;
	CadMath::Vector4 calculateNormal(float x, float y, float z) const;
public:
	int currentSizeX, currentSizeY;
	RGB* image = nullptr;
	Transform temporaryTransformation;
	Elipse();
	~Elipse();
	void setRadious(float x, float y, float z);
	RGB* createTexture(int sizeX, int sizeY);
	void updateTransforamtions();
	void applyTemporaryTransformation();
	void renderUI();
};