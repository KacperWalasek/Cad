#include "CadMath.h"
#include <exception>

std::vector<glm::fvec3> CadMath::SolveTridiagonalMatrix(std::vector<glm::fvec3> a1, std::vector<glm::fvec3> a2, std::vector<glm::fvec3> a3, std::vector<glm::fvec3> r)
{
	int n = r.size();
	std::vector<glm::fvec3> result(n);
	for (int i = 1; i < n; i++)
	{
		glm::fvec3 w = a1[i-1] / a2[i-1];
		a2[i] -= a3[i - 1] * w;
		r[i] -= r[i - 1] * w;
	}
	result[n - 1] = r[n-1]/a2[n-1];

	for (int i = n - 2; i >= 0; i--)
		result[i] = (r[i] - result[i + 1] * a3[i]) / a2[i];

	return result;
}

std::vector<glm::fvec4> CadMath::PowerToBernstain(std::vector<glm::fvec3> powerCoords)
{
	if (powerCoords.size() != 4)
		throw;
	
	glm::fmat4x4 mat = {
		1,1,1,1,
		0,1/3.0f,2/3.0f, 1,
		0,0,1/3.0f,1,
		0,0,0,1
	};
	
	std::vector<glm::fvec4> polynomials = { 
		{
			powerCoords[0].x,
			powerCoords[1].x,
			powerCoords[2].x,
			powerCoords[3].x
		},
		{
			powerCoords[0].y,
			powerCoords[1].y,
			powerCoords[2].y,
			powerCoords[3].y
		},
		{
			powerCoords[0].z,
			powerCoords[1].z,
			powerCoords[2].z,
			powerCoords[3].z
		} 
	};

	for (int i = 0; i < 3; i++)
		polynomials[i] = mat * polynomials[i];

	std::vector<glm::fvec4> bezierCoords = {
		{
			polynomials[0].x,
			polynomials[1].x,
			polynomials[2].x,
			1
		},
		{
			polynomials[0].y,
			polynomials[1].y,
			polynomials[2].y,
			1
		},
		{
			polynomials[0].z,
			polynomials[1].z,
			polynomials[2].z,
			1
		},
		{
			polynomials[0].w,
			polynomials[1].w,
			polynomials[2].w,
			1
		}

	};

	return bezierCoords;
}
