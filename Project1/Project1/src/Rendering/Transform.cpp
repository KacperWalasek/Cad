#include "Transform.h"
#include <math.h>

glm::fmat4x4 Transform::GetMatrix() const
{
	if(reversedOrder)
		return GetScaleMatrix() * GetRotationMatrix() * GetLocationMatrix();
	return GetLocationMatrix() * GetRotationMatrix() * GetScaleMatrix();
}

glm::fmat4x4 Transform::GetInverseMatrix() const
{
	Transform inverse(-location, -rotation, 1.0f / scale);
	if(reversedOrder)
		return  inverse.GetLocationMatrix() * inverse.GetInverseOrderRotationMatrix() *inverse.GetScaleMatrix();
	return inverse.GetInverseOrderRotationMatrix() * inverse.GetLocationMatrix() * inverse.GetScaleMatrix();

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
	return rotationZ*rotationY*rotationX;
}

glm::fmat4x4 Transform::GetInverseOrderRotationMatrix() const
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
	return rotationX * rotationY * rotationZ;
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
	Transform ret = Transform(location + t.location, rotation + t.rotation, scale * t.scale);
	ret.reversedOrder = reversedOrder;
	return ret;
}

void Transform::RenderGui()
{
	float locationTmp[3] = {location[0],location[1], location[2]};
	float rotationTmp[3] = { rotation[0],rotation[1], rotation[2] };
	float scaleTmp[3] = { scale[0],scale[1], scale[2] };
	if (ImGui::InputFloat3("Location", locationTmp))
		location = { locationTmp[0],locationTmp[1], locationTmp[2],0 };
	if (ImGui::InputFloat3("Rotation", rotationTmp))
		rotation = { rotationTmp[0],rotationTmp[1], rotationTmp[2],0 };
	if (ImGui::InputFloat3("Scale", scaleTmp))
	{
		if (scaleTmp[0] <= 0)
			scaleTmp[0] = 0.01f;
		if (scaleTmp[1] <= 0)
			scaleTmp[1] = 0.01f;
		if (scaleTmp[2] <= 0)
			scaleTmp[2] = 0.01f;

		scale = { scaleTmp[0],scaleTmp[1], scaleTmp[2],0 };
	}
}
