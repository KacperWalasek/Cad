#pragma once
#include <vector>
#include <glm/glm.hpp>

class CadMath
{
public:
	static std::vector<glm::fvec3> SolveTridiagonalMatrix(std::vector<glm::fvec3> a1, std::vector<glm::fvec3> a2, std::vector<glm::fvec3> a3, std::vector<glm::fvec3> r);
	static std::vector<glm::fvec4> PowerToBernstain(std::vector<glm::fvec3> powerCoords );
};