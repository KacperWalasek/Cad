#pragma once
#include <array>
#include <gl/glew.h>
#include "../../interfaces/IRenderable.h"
#include "../../Rendering/Shader.h"
#include "../../Rendering/ProceduralTexture.h"

class MaterialCube : public IRenderable {
	Shader heightMapShader;
	unsigned int heightMapTexture;
	unsigned int VBO, EBO, VAO;

	unsigned int pathsVBO, pathsEBO, pathsVAO;
	void createHeightMapQuad();
public:
	MaterialCube();
	void applyMap() const;
	void setTexture(const ProceduralTexture& texture);


	// Inherited via IRenderable
	virtual void Render(bool selected, VariableManager& vm) override;

};