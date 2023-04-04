#pragma once
#include "../Rendering/Mesh.h"
#include <glm/glm.hpp>
#include "../Rendering/Shader.h"


class BezierCurve
{
public:
	Mesh curveMesh;
	Mesh chainMesh;
	void UpdateMeshes(std::vector<glm::fvec4> points);
	void Render(Shader& shader);
};