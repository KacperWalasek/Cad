#include "Sampler.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "glm/glm.hpp"

Mesh Sampler::sampleTorus(const TorusGeometry& torus, int majorDivision, int minorDivision) const
{
	Mesh mesh;
	for (int i = 0; i < majorDivision; i++)
	{
		for (int j = 0; j < minorDivision; j++)
		{
			glm::fvec4 v = torus.getPointOnMesh(i * 2 * M_PI / (float)majorDivision, j * 2 * M_PI / (float)minorDivision);
			mesh.vertices.push_back(v.x);
			mesh.vertices.push_back(v.y);
			mesh.vertices.push_back(v.z);

			mesh.indices.push_back(i*minorDivision+j);
			mesh.indices.push_back(i * minorDivision + (j+1)%minorDivision);

			mesh.indices.push_back(i * minorDivision + j);
			mesh.indices.push_back(((i+1)%majorDivision) * minorDivision + j);

			//mesh.indices.push_back(i * minorDivision + (j + 1) % minorDivision);
			//mesh.indices.push_back(((i + 1) % majorDivision) * minorDivision + j);
		}
	}
	return mesh;
}

Mesh Sampler::sampleBezierCurve(const BezierCurve& curve, int division)
{
	Mesh mesh;
	for (int i = 0; i < division; i++)
	{
		glm::fvec4 v = curve.getPointOnCurve((float)i / (float)(division-1));
		mesh.vertices.push_back(v.x);
		mesh.vertices.push_back(v.y);
		mesh.vertices.push_back(v.z);

		if (i < division - 1)
		{
			mesh.indices.push_back(i);
			mesh.indices.push_back(i + 1);
		}
	}
	return mesh;
}
