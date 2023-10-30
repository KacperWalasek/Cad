#pragma once
#include "MillingPath.h"
#include "../../interfaces/IRenderable.h"
#include "../../Rendering/Shader.h"

class MillingPathVisualizer : public IRenderable
{
	Shader shader;
	unsigned int VAO, VBO, EBO;
	int indsCount = 0;
public:
	MillingPathVisualizer();
	void setMillingPath(const MillingPath& path);
	

	// Inherited via IRenderable
	virtual void Render(bool selected, VariableManager& vm) override;

};