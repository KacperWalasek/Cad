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
	std::vector<int> indsTriangles = {
		0,1,3,
		0,3,2
	};
	glGenVertexArrays(1, &VAO_quads);
	glGenVertexArrays(1, &VAO_triangles);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO_quad);
	glGenBuffers(1, &EBO_triangle);

	// create vbo for both vao
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

	// create vao for tesselation
	glBindVertexArray(VAO_quads);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_quad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	// create vao for bottom wall
	glBindVertexArray(VAO_triangles);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_triangle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indsTriangles.size() * sizeof(int), &indsTriangles[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void MaterialCube::renderBottom(VariableManager& vm)
{
	phongShader.use();
	vm.SetVariable("color", glm::fvec4(0, 1, 0, 0));
	vm.SetVariable("modelMtx", glm::identity<glm::fmat4x4>());
	glBindVertexArray(VAO_triangles);
	vm.Apply(phongShader.ID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void MaterialCube::renderTop(VariableManager& vm)
{
	heightMapShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMapTexture);
	vm.SetVariable("color", glm::fvec4(0, 1, 0, 0));
	vm.SetVariable("modelMtx", glm::identity<glm::fmat4x4>());
	for (int i = 0; i < divisionX / 100.0f; i++)
		for (int j = 0; j < divisionX / 100.0f; j++)
		{
			vm.SetVariable("offsetUBeg", i * 100 / (float)divisionX);
			vm.SetVariable("offsetUEnd", (i + 1) * 100 / (float)divisionX);
			vm.SetVariable("offsetVBeg", j * 100 / (float)divisionY);
			vm.SetVariable("offsetVEnd", (j + 1) * 100 / (float)divisionY);
			vm.SetVariable("divisionU", 100);
			vm.SetVariable("divisionV", 100);
			vm.Apply(heightMapShader.ID);
			glBindVertexArray(VAO_quads);
			glPatchParameteri(GL_PATCH_VERTICES, 4);
			glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT, 0);
		} 
}   
   
void MaterialCube::renderWalls(VariableManager& vm)
{  
	wallsShader.use(); 
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, heightMapTexture);
	vm.SetVariable("color", glm::fvec4(0, 1, 0, 0));
	vm.SetVariable("modelMtx", glm::identity<glm::fmat4x4>());
	for (int i = 0; i < 4; i++) { 
		vm.SetVariable("index", i);
		for (int i = 0; i < divisionX / 100.0f; i++)
		{
			vm.SetVariable("offsetUBeg", i * 100 / (float)divisionX);
			vm.SetVariable("offsetUEnd", (i + 1) * 100 / (float)divisionX);
			vm.SetVariable("divisionU", 100);
			vm.Apply(wallsShader.ID);
			glBindVertexArray(VAO_quads);
			glPatchParameteri(GL_PATCH_VERTICES, 4);
			glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT, 0);
		}
	}
}

MaterialCube::MaterialCube(int divisionX, int divisionY)
	: heightMapShader("Shaders/HeightMap/heightMap.vert","Shaders/HeightMap/heightMap.frag"),
	phongShader("Shaders/Phong/phong.vert", "Shaders/Phong/phong.frag"),
	wallsShader("Shaders/HeightMap/heightMap.vert", "Shaders/HeightMap/heightMap.frag"),
	divisionX(divisionX), divisionY(divisionY)
{
	heightMapShader.Init();
	heightMapShader.loadShaderFile("Shaders/HeightMap/heightMap.tesc", GL_TESS_CONTROL_SHADER);
	heightMapShader.loadShaderFile("Shaders/HeightMap/heightMap.tese", GL_TESS_EVALUATION_SHADER);

	phongShader.Init();
	wallsShader.Init();
	wallsShader.loadShaderFile("Shaders/HeightMap/wall.tesc", GL_TESS_CONTROL_SHADER);
	wallsShader.loadShaderFile("Shaders/HeightMap/wall.tese", GL_TESS_EVALUATION_SHADER);
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
	renderBottom(vm);
	renderWalls(vm);
	renderTop(vm);

}
