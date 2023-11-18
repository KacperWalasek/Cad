#pragma once
#include <array>
#include <gl/glew.h>
#include "../../interfaces/IRenderable.h"
#include "../../Rendering/Shader.h"
#include "../../Rendering/ProceduralTexture.h"
#include "../../Rendering/TextureRenderer.h"

class MaterialCube : public IRenderable {
	Shader heightMapShader, phongShader, wallsShader;
	unsigned int heightMapTexture;
	unsigned int VBO, EBO_quad, EBO_triangle, VAO_quads, VAO_triangles;

	unsigned int pathsVBO, pathsEBO, pathsVAO;

	int divisionX, divisionY;

	glm::fmat4x4 modelMatrix;
	float height = 0;
	void createHeightMapQuad();


	void renderBottom(VariableManager& vm);
	void renderTop(VariableManager& vm);
	void renderWalls(VariableManager& vm);
public:
	MaterialCube(int divisionX, int divisionY);
	void applyMap() const;
	void setTexture(unsigned int texture);
	void setSize(float x, float y, float z);
	void setDivision(int sizeX, int sizeY);
	// Inherited via IRenderable
	virtual void Render(bool selected, VariableManager& vm) override;

};