#include "Rotator.h"
#include <corecrt_math_defines.h>
#define _USE_MATH_DEFINES
#include <cmath>

glm::fmat4x4 Rotator::GetRotationMatrix(glm::fvec4 vector, float angle) 
{
	vector = normalize(vector);
	glm::fmat4x4 W(
		0.0f , vector.z, - vector.y,0.0f,
		-vector.z, 0.0f, vector.x,0.0f,
		vector.y, - vector.x, 0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f
	);

	return glm::fmat4x4(1.0f) + sinf(angle)*W + (1-cosf(angle))*W*W;
}

glm::fvec4 Rotator::AxisToEuler(glm::fvec4 vector, float angle)
{
	glm::fmat4x4 r = GetRotationMatrix(vector, angle);
	return MatrixToEuler(r);
}

glm::fvec4 Rotator::MatrixToEuler(glm::fmat4x4 r)
{
	if (fabs(r[0][2]) != 1)
	{
		float y = asinf(r[2][0]);
		float x = atan2f(r[1][2] / cosf(y), r[2][2] / cosf(y));
		float z = atan2f(r[0][1] / cosf(y), r[0][0] / cosf(y));
		return { x,y,z,0.0f };
	}
	else
	{
		float z = 0;
		if (r[0][2] == -1)
		{
			float y = -M_PI / 2.0f;
			float x = z + atan2f(r[1][0], r[2][0]);
			return { x,y,z,0.0f };
		}
		else
		{
			float y = M_PI / 2.0f;
			float x = -z + atan2f(-r[1][0], -r[2][0]);
			return { x,y,z,0.0f };
		}
	}
}
