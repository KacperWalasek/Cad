#include "Sampler.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "glm/glm.hpp"

Mesh Sampler::sampleTorus(const Torus& torus, int majorDivision, int minorDivision) const
{
	Mesh mesh;
	std::vector<std::vector<glm::fvec4>> vert;
	for (int i = 0; i < majorDivision; i++)
	{
		std::vector<glm::fvec4> ring;
		for (int j = 0; j < minorDivision; j++)
		{
			glm::fvec4 v = torus.getPointOnMesh(i * 2 * M_PI / (float)minorDivision, j * 2 * M_PI / (float)majorDivision);
			ring.push_back(v);
			// TODO : calculate indices
		}
		vert.push_back(ring);
	}
	// TODO : Flatten vertices and indices
	return mesh;
}
