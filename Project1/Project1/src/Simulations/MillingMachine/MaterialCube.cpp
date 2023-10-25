#include "MaterialCube.h"

void MaterialCube::createHeightMapQuad()
{
	std::vector<float> vert = {
		0,0,0,
		0,0,1,
		1,0,0,
		1,0,1,
	};
	std::vector<int> inds = {
		0,1,2,3
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

MaterialCube::MaterialCube(int divisionX, int divisionY)
	: heightMapShader("Shaders/HeightMap/heightMap.vert","Shaders/HeightMap/heightMap.frag"),
	divisionX(divisionX), divisionY(divisionY)
{
	heightMapShader.Init();
	heightMapShader.loadShaderFile("Shaders/HeightMap/heightMap.tesc", GL_TESS_CONTROL_SHADER);
	heightMapShader.loadShaderFile("Shaders/HeightMap/heightMap.tese", GL_TESS_EVALUATION_SHADER);
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
	for(int i = 0; i < divisionX/100.0f; i++)
		for (int j = 0; j < divisionX / 100.0f; j++)
		{
			vm.SetVariable("offsetUBeg", i * 100 / (float)divisionX);
			vm.SetVariable("offsetUEnd", (i + 1) * 100 / (float)divisionX);
			vm.SetVariable("offsetVBeg", j * 100 / (float)divisionY);
			vm.SetVariable("offsetVEnd", (j + 1) * 100 / (float)divisionY);
			vm.SetVariable("divisionU", 100);
			vm.SetVariable("divisionV", 100);
			vm.Apply(heightMapShader.ID);
			glBindVertexArray(VAO);
			glPatchParameteri(GL_PATCH_VERTICES, 4);
			glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT, 0);
		}
}
