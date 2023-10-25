#pragma once
#include "Shader.h"
#include "../interfaces/IRenderable.h"
#include <memory>

class TextureRenderer {
	int sizeX, sizeY, chanels; 
	
	unsigned int tex, fb, depth;
	void createTexture(bool depthTest);
public:
	TextureRenderer(int sizeX, int sizeY, int chanels, bool depthTest);
	unsigned int getTextureId() const;

	void Clear(glm::fvec4 color);
	void Render(IRenderable& object, VariableManager& variableManager);
}; 