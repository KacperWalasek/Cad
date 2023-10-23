#include "MaterialCube.h"

void MaterialCube::createHeightMapQuad()
{
	std::vector<float> vert = {
		0,0,0,
		0,0,1,
		1,0,0,
		1,0,1
	};
	std::vector<int> inds = {
		0,2,1,
		1,3,2
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

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

MaterialCube::MaterialCube()
	: heightMapShader("Shaders/texture.vert","Shaders/texture.frag")
{
	heightMapShader.Init();
	createHeightMapQuad();
}

void MaterialCube::applyMap() const
{
}

void MaterialCube::setTexture(unsigned int texture)
{
	heightMapTexture = texture;
}

void MaterialCube::Render(bool selected, VariableManager& vm)
{
	heightMapShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMapTexture);
	vm.SetVariable("color", glm::fvec4(0, 1, 0, 0));
	vm.SetVariable("modelMtx", glm::identity<glm::fmat4x4>());
	vm.Apply(heightMapShader.ID);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, 0);
}
