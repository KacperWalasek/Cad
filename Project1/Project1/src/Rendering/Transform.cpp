#include "Transform.h"
#include <math.h>

glm::fmat4x4 Transform::GetMatrix() const
{
	return GetScaleMatrix()*GetRotationMatrix()* GetLocationMatrix();
}

glm::fmat4x4 Transform::GetInverseMatrix() const
{
	return glm::fmat4x4();
}

glm::fmat4x4 Transform::GetRotationMatrix() const
{
	glm::fmat4x4 rotationX = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,cosf(rotation.x),sinf(rotation.x),0.0f,
		0.0f,-sinf(rotation.x), cosf(rotation.x),0.0f,
		0.0f,0.0f,0.0f,1.0f };
	glm::fmat4x4 rotationY = {
	    cosf(rotation.y),0.0f,sinf(rotation.y),0.0f,
		0.0f,1.0f,0.0f,0.0f,
		-sinf(rotation.y),0.0f,cosf(rotation.y),0.0f,
		0.0f,0.0f,0.0f,1.0f };
	glm::fmat4x4 rotationZ = {
	    cosf(rotation.z),sinf(rotation.z),0.0f,0.0f,
		-sinf(rotation.z), cosf(rotation.z),0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f };
	return rotationX*rotationY*rotationZ;
}

glm::fmat4x4 Transform::GetLocationMatrix() const
{
	return { 
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		location.x,location.y,location.z,1.0f};
}
glm::fmat4x4 Transform::GetScaleMatrix() const
{
	return { 
		scale.x,0.0f,     0.0f,     0.0f,
		0.0f,     scale.y,0.0f,     0.0f,
		0.0f,     0.0f,     scale.z,0.0f,
		0.0f,     0.0f,     0.0f,     1.0f };
}

Transform Transform::operator+(const Transform& t)
{
	return Transform(location + t.location, rotation + t.rotation, scale * t.scale);
}
