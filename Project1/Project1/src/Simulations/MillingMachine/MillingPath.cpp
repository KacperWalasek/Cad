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
	times = { 0 };
	times.reserve(positions.size());
	for (int i = 1; i < positions.size(); i++) {
		float len = (positions[i] - positions[i - 1]).length();
		times.push_back(times[i - 1] + len);
		totalLength += len;
	}
	for (int i = 0; i < positions.size(); i++)
		times[i] /= totalLength;
}

MillingPath::MillingPath(std::stringstream& ss, float radius, bool flat)
	: radius(radius), flat(flat)
{
	readPositions(ss);
	update();
}
