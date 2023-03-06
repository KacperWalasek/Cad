#pragma once
#include <functional>
#include "RGB.h"
#include <future>

class AdaptiveRendering
{
	std::function<RGB(float x, float y)> calculatePixel;
	std::future<RGB*> job;
	unsigned int VAO, shaderProgram, texName;
	RGB* createTexture(int sizeX, int sizeY);
	int currentSizeY;
	int aimedSizeX;
	int aimedSizeY;
	bool reset = false;
public:
	AdaptiveRendering(std::function<RGB(float x, float y)> calculatePixel, int maxSizeX, float  viewportRatio, int  initSizeX, int stepX);
	~AdaptiveRendering();
	bool hardReset = false;
	int initSizeX;
	int currentSizeX;
	int maxSizeX;
	float viewportRatio;
	int stepX;
	RGB* currentTexture;

	void Init();
	bool Checkout();
	void Reset();
	void Render();
};
