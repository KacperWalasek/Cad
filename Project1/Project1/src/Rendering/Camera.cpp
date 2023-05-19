#include "Camera.h"
#define _USE_MATH_DEFINES
#include <math.h>
Camera::Camera(Window& window, float fov, float n, float f)
	: window(window), fov(fov), n(n), f(f)
{
	transform.reversedOrder = true;
}

glm::fmat4x4 Camera::GetViewMatrix() const
{
	return transform.GetInverseMatrix();
}

glm::fmat4x4 Camera::GetProjectionMatrix() const
{
	float aspect = GetAspect();
	float ctg = 1 / tanf(fov / 2);
	
	return {
		ctg/aspect, 0,0,0,
		0,ctg,0,0,
		0,0,(f+n)/(f-n),1,
		0,0,(-2*f*n)/(f-n),0
	};
}

glm::fmat4x4 Camera::GetInverseProjectionMatrix() const
{
	float aspect = GetAspect();
	float ctg = 1 / tanf(fov / 2);

	return {
		aspect/ctg, 0,0,0,
		0,1/ctg,0,0,
		0,0,0,(f - n)/ (-2 * f * n),
		0,0, 1 ,-(f + n) / (-2 * f * n)
	};
}


bool Camera::RenderGui()
{
	float eps = 0.01f;
	ImGui::Begin("Camera");
	transform.RenderGui();
	ImGui::SliderFloat("fov", &fov, eps, M_PI - eps);
	ImGui::SliderFloat("near", &n, eps, f-eps);
	if (ImGui::InputFloat("far", &f))
		if (f <= n)
			f = n + 0.1f;
	ImGui::End();
	return false;
}

float Camera::GetAspect() const
{
	return window.sizeX / window.sizeY;
}
