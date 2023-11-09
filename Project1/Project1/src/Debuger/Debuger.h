#pragma once
#include <vector>
#include "../interfaces/IGui.h"
#include "../interfaces/IRenderable.h"
#include "../Rendering/Shader.h"

class Debuger : public IGui, public IRenderable
{
	static std::vector<glm::fvec2> uvPoints;
	static unsigned int VAO, VBO, EBO;
	static Shader uvPointShader;

	static std::vector<unsigned int> textures;
	static void updateUVPoints();
public:
	static void Init();

	static void ShowTexture(unsigned int tex);
	static void ShowUVPoint(glm::fvec2 point);

	// Inherited via IGui
	virtual bool RenderGui() override;

	// Inherited via IRenderable
	virtual void Render(bool selected, VariableManager& vm) override;
};