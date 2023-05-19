#include "AnaglyphCamera.h"

AnaglyphCamera::AnaglyphCamera(Camera& baseCamera, float translationX)
	:baseCamera(baseCamera), translationX(translationX), dist(50.0f)
{}

void AnaglyphCamera::SetTranslationX(float translationX) 
{
	this->translationX = translationX;
}
glm::fmat4x4 AnaglyphCamera::GetViewMtx()
{
	return glm::fmat4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-translationX, 0.0f, 0.0f, 1.0f)*baseCamera.GetViewMatrix();
}

glm::fmat4x4 AnaglyphCamera::GetProjectionMtx() 
{
	float fov = baseCamera.fov;
	float n = baseCamera.n;
	float f = baseCamera.f;
	float y = tanf(fov / 2) * n;
	float x = y * baseCamera.GetAspect();
	float diff = -translationX * n / dist;

	float l = -x - diff;
	float r = x - diff;
	float t = y;
	float b = -y;

	return {
		2 * n/(r-l), 0.0f,0.0f,0.0f,
		0.0f, 2*n/(t-b), 0.0f,0.0f,
		(r+l)/(r-l), (t+b)/(t-b), (f+n)/(f-n), 1.0f,
		0.0f,0.0f,-2*f*n/(f-n),0.0f
	};
}