#pragma once
#include <sstream>
#include <vector>
#include <glm/gtc/type_precision.hpp>

class MillingPath {
	float getCoord(std::string line, char coord);
	void readPositions(std::stringstream& ss);
	void update();
public:
	std::vector<glm::fvec3> positions;
	glm::fvec2 zRange;
	float totalLength = 0;
	std::vector<float> dists;
	bool flat;
	float radius;

	MillingPath(std::stringstream& ss, float radius, bool flat);
	MillingPath() = default;
};