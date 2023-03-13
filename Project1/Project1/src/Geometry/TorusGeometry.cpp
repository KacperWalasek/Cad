#include "TorusGeometry.h"

TorusGeometry::TorusGeometry()
	: majorR(1), minorR(0.3f)
{
}

TorusGeometry::TorusGeometry(float majorR, float minorR)
	: majorR(majorR), minorR(minorR)
{
}

glm::fvec4 TorusGeometry::getPointOnMesh(float majorAngle, float minorAngle) const
{
	return glm::vec4(majorR * glm::fvec3(cosf(majorAngle), 0, sinf(majorAngle)), 1.0f) + minorR * glm::mat4x4(
		cosf(majorAngle), 0.0f, sinf(majorAngle), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sinf(majorAngle), 0.0f, cosf(majorAngle), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f) * glm::fvec4(cosf(minorAngle), sinf(minorAngle), 0, 0);
}
