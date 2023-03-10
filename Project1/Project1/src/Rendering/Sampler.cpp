#include "Sampler.h"
#include "../Math/Vector4.h"
#define _USE_MATH_DEFINES
#include <math.h>

Mesh Sampler::sampleTorus(const Torus& torus, int majorDivision, int minorDivision) const
{
	Mesh mesh;
	std::vector<std::vector<CadMath::Vector4>> vert;
	for (int i = 0; i < majorDivision; i++)
	{
		std::vector<CadMath::Vector4> ring;
		for (int j = 0; j < minorDivision; j++)
		{
			CadMath::Vector4 v = torus.getPointOnMesh(i * 2 * M_PI / (float)minorDivision, j * 2 * M_PI / (float)majorDivision);
			ring.push_back(v);
			// TODO : calculate indices
		}
		vert.push_back(ring);
	}
	// TODO : Flatten vertices and indices
	return mesh;
}
