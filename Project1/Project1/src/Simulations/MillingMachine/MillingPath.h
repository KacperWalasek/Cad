#pragma once
#include <sstream>
#include <vector>
#include <glm/gtc/type_precision.hpp>

class MillingPath {
	float getCoord(std::string line, char coord);
public:
	std::vector<glm::fvec3> positions;

	MillingPath(std::stringstream& ss);
};