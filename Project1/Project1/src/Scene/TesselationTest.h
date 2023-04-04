#pragma once
#include "../interfaces/IRenderable.h"
#include "ISceneElement.h"
#include "../Rendering/Shader.h"
#include "../Rendering/Camera.h"

class TesselationTest : public IRenderable, public ISceneElement
{
	unsigned int VAO, VBO, EBO;
	Camera& camera;
public:
	TesselationTest(Camera& camera);
	
	Shader shader;

	// Inherited via ISceneElement
	virtual std::string getName() const override;


	// Inherited via IRenderable
	virtual void Render() override;

};