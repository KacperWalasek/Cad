#include "MillingPath.h"

float MillingPath::getCoord(std::string line, char coord)
{
	int indX = line.find(coord);
	if (indX == -1)
		return 0.0f;
	int end = line.find('.', indX)+3;
	return std::stof(line.substr(indX+1, end-indX));
}

MillingPath::MillingPath(std::stringstream& ss)
{
	std::string line;
	while (std::getline(ss, line))
		positions.push_back({
			getCoord(line,'X'),
			getCoord(line,'Y'),
			getCoord(line,'Z') 
		});
}
