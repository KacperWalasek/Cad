#pragma once
#include "../../Rendering/Mesh.h"
#include <glm/glm.hpp>
#include "../../Rendering/Shader.h"
#include "../../Rendering/VariableManager.h"
#include <array>

class BezierCurve
{
public:
	static glm::fvec3 deCastljeu(float t, std::array<glm::fvec3, 4> p);
	static glm::fvec3 deCastljeu(float t, std::array<glm::fvec3, 3> p);

	Mesh curveMesh;
	Mesh chainMesh;
	void UpdateMeshes(std::vector<glm::fvec4> points);
	void Render(Shader& shader, VariableManager& vm);
};