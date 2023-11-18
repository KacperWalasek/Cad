#pragma once
#include <vector>
#include <glm/glm.hpp>

class TextureCPUAccess {
	int sizeX, sizeY;
	unsigned int tex;
	std::vector<float> data;
public:
	TextureCPUAccess(unsigned int tex, int sizeX, int sizeY);
	glm::fvec3 getByIndex(int X, int Y) const;
	float getXByIndex(int X, int Y) const;
	glm::fvec3 getByUV(glm::fvec2 uv) const;

	void Update();
};