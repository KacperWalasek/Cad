#include "Camera.h"
#define _USE_MATH_DEFINES
#include <math.h>
Camera::Camera(Window& window, float fov, float n, float f)
	: window(window), fov(fov), n(n), f(f)
{}

glm::fmat4x4 Camera::GetViewMatrix() const
{
	return transform.GetInverseMatrix();
}

glm::fmat4x4 Camera::GetProjectionMatrix() const
{
	float aspect = window.sizeX /window.sizeY;
	float ctg = 1 / tanf(180/M_PI *fov / 2);
	
	return {
		ctg/aspect, 0,0,0,
		0,ctg,0,0,
		0,0,(f+n)/(f-n),1,
		0,0,(-2*f*n)/(f-n),0
	};
}
