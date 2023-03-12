#pragma once
#include <vector>

class Mesh
{
public:
	unsigned int VBO, EBO, VAO;
	std::vector<float> vertices;
	std::vector<int> indices;

	void Init();
	void Render();
};
