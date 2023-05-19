#pragma once
#include <glm/glm.hpp>
#include "../Camera.h"

class AnaglyphCamera
{
	Camera& baseCamera;
public:
	float translationX;
	float dist;
	AnaglyphCamera(Camera& baseCamera, float translationX = 0.0f);

	void SetTranslationX(float translationX);
	glm::fmat4x4 GetViewMtx();
	glm::fmat4x4 GetProjectionMtx();
};