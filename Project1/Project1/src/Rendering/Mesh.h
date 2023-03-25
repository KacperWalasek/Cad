#pragma once
#include <vector>

class Mesh
{
public:
	static Mesh CombineMeshes(const Mesh& mesh1, const Mesh& mesh2);
	Mesh();

	unsigned int VBO, EBO, VAO;
	std::vector<float> vertices;
	std::vector<int> indices;

	void Update();
	void Render();
};


