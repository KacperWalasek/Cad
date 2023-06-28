#include "BezierCurve.h"

glm::fvec3 BezierCurve::deCastljeu(float t, std::array<glm::fvec3, 4> p)
{
	float nt = 1 - t;

	p[3] = p[3] * t + p[2] * nt;
	p[2] = p[2] * t + p[1] * nt;
	p[1] = p[1] * t + p[0] * nt;

	p[3] = p[3] * t + p[2] * nt;
	p[2] = p[2] * t + p[1] * nt;

	p[3] = p[3] * t + p[2] * nt;

	return p[3];
}

glm::fvec3 BezierCurve::deCastljeu(float t, std::array<glm::fvec3, 3> p)
{
	float nt = 1 - t;

	p[2] = p[2] * t + p[1] * nt;
	p[1] = p[1] * t + p[0] * nt;

	p[2] = p[2] * t + p[1] * nt;

	return p[2];
}
void BezierCurve::UpdateMeshes(std::vector<glm::fvec4> points)
{
	chainMesh = Mesh();
	curveMesh = Mesh();
	if (points.size() > 1)
	{
		std::vector<float> vert;
		std::vector<int> ind;

		for (int i = 0; i < points.size(); i++)
		{
			chainMesh.vertices.push_back(points[i].x);
			chainMesh.vertices.push_back(points[i].y);
			chainMesh.vertices.push_back(points[i].z);

			if(i!=0)
			{
				chainMesh.indices.push_back(i-1);
				chainMesh.indices.push_back(i);
			}
		}
		while (points.size() != 4)
		{
			float m = points.size();
			points.push_back(points[m - 1]);
			for (int i = m - 1; i > 0; i--)
				points[i] = (i / m) * points[i - 1] + ((m - i) / m) * points[i];
		}
		for (int i = 0; i < points.size(); i++)
		{
			curveMesh.vertices.push_back(points[i].x);
			curveMesh.vertices.push_back(points[i].y);
			curveMesh.vertices.push_back(points[i].z);
			curveMesh.indices.push_back(i);
		}
	}
	chainMesh.Update();
	curveMesh.Update();
}

void BezierCurve::Render(Shader& shader, VariableManager& vm)
{
	glBindVertexArray(curveMesh.VAO);
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	float baseDivision = 4.0f;
	for (int i = 0; i < baseDivision; i++)
	{
		vm.SetVariable("t0", i / baseDivision);
		vm.SetVariable("t1", (i + 1) / baseDivision);
		vm.Apply(shader.ID);
		glDrawElements(GL_PATCHES, curveMesh.indices.size(), GL_UNSIGNED_INT, 0);
	}
}