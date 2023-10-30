#include "MillingPathVisualizer.h"
#include <GL/glew.h>

MillingPathVisualizer::MillingPathVisualizer()
	: shader("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag")
{
	shader.Init();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

}

void MillingPathVisualizer::setMillingPath(const MillingPath& path)
{
	std::vector<float> vert;
	std::vector<int> inds;
	for (int i = 0; i<path.positions.size(); i++)
	{
		vert.insert(vert.end(), {
			path.positions[i].x, path.positions[i].y, path.positions[i].z
			});
		if(i!=0)
			inds.insert(inds.end(), { i-1, i });
	}
	indsCount = inds.size();

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MillingPathVisualizer::Render(bool selected, VariableManager& vm)
{
	if (indsCount == 0)
		return;
	shader.use();
	vm.SetVariable("color", glm::fvec4(1.0f, 0.0f, 0.0f, 1.0f));
	vm.SetVariable("modelMtx", glm::identity<glm::fmat4x4>());
	vm.Apply(shader.ID);
	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, indsCount, GL_UNSIGNED_INT, 0);
}
