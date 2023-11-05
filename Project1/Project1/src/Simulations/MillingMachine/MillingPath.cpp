#include "MillingPath.h"

float MillingPath::getCoord(std::string line, char coord)
{
	int indX = line.find(coord);
	if (indX == -1)
		return 0.0f;
	int end = line.find('.', indX)+3;
	return std::stof(line.substr(indX+1, end-indX));
}

void MillingPath::readPositions(std::stringstream& ss)
{
	std::string line;
	while (std::getline(ss, line))
		positions.push_back({
			getCoord(line,'X'),
			getCoord(line,'Y'),
			getCoord(line,'Z')
			});
}

std::string MillingPath::serializeNumber(float c) const
{
	int intSection = floorf(c);
	int floatSection = floorf((c - intSection) * 1000.0f);
	return std::to_string(intSection) + "." + std::to_string(floatSection);
}

std::stringstream MillingPath::serialize() const
{
	std::stringstream ss;
	for (int i = 0; i < positions.size(); i++)
	{
		ss << "N" << i + 2 << "G01" <<
			"X" << serializeNumber(positions[i].x) <<
			"Y" << serializeNumber(positions[i].y) <<
			"Z" << serializeNumber(positions[i].z) <<
			std::endl;
	}
	return ss;
}

void MillingPath::update()
{
	totalLength = 0;
	dists = { 0 };
	if (positions.empty())
		return;
	dists.reserve(positions.size());
	for (int i = 1; i < positions.size(); i++) {
		glm::fvec3 dist = positions[i] - positions[i - 1];
		float len = glm::fvec2(dist.x,dist.y).length();
		dists.push_back(dists[i - 1] + len);
		totalLength += len;
	}
	for (int i = 0; i < positions.size(); i++)
		dists[i] /= totalLength;

	zRange = { positions[0].z,30 };
	for (const auto& p : positions) 
	{
		if (p.z < zRange.x)
			zRange.x = p.z;
		if (p.z > zRange.y)
			zRange.y = p.z;
	}
}

MillingPath::MillingPath(std::stringstream& ss, float radius, bool flat)
	: radius(radius), flat(flat)
{
	readPositions(ss);
	update();
}

MillingPath::MillingPath(std::vector<glm::fvec3> positions, float radius, bool flat)
	: positions(positions)
{
	update();
}
