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

void MillingPath::update()
{
	totalLength = 0;
	dists = { 0 };
	if (positions.empty())
		return;
	dists.reserve(positions.size());
	for (int i = 1; i < positions.size(); i++) {
		float len = (positions[i] - positions[i - 1]).length();
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
