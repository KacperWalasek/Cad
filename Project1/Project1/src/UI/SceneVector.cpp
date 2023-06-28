#include "SceneVector.h"

SceneVector::SceneVector(glm::fvec3 position, glm::fvec3 end, glm::fvec4 color)
	: shader("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag"),
	color(color)
{
	shader.Init();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	
	std::vector<float> vert = {
		position.x,position.y, position.z,
		end.x,end.y,end.z
	};
	
	std::vector<int> ind = { 0,1 };

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(int), &ind[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

}

void SceneVector::Render(bool selected, VariableManager& vm)
{
	shader.use();
	vm.SetVariable("color", color);
	vm.Apply(shader.ID);
	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
}
