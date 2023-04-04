#include "BezierCurve.h"

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

void BezierCurve::Render(Shader& shader)
{
	glBindVertexArray(curveMesh.VAO);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	unsigned int t0Loc = glGetUniformLocation(shader.ID, "t0");
	unsigned int t1Loc = glGetUniformLocation(shader.ID, "t1");
	float baseDivision = 4.0f;
	for (int i = 0; i < baseDivision; i++)
	{
		glUniform1f(t0Loc, i / baseDivision);
		glUniform1f(t1Loc, (i+1) / baseDivision);
		glDrawElements(GL_PATCHES, curveMesh.indices.size(), GL_UNSIGNED_INT, 0);
	}
}