#pragma once
#include <vector>
#include "../interfaces/IGui.h"
#include "../interfaces/IRenderable.h"
#include "../Rendering/Shader.h"
#include "../interfaces/IUVSurface.h"
#include "../Rendering/Camera.h"

class Debuger : public IGui, public IRenderable
{
	static std::vector<glm::fvec2> uvPoints;
	static std::vector<glm::fvec3> points;
	static std::vector<std::pair<glm::fvec3, glm::fvec3>> vectors;
	static unsigned int uvVAO, uvVBO, uvEBO, pointVAO,pointVBO,pointEBO, vecVAO, vecVBO, vecEBO;
	static Shader uvPointShader, pointShader;

	static std::vector<unsigned int> textures;
	static std::vector<std::shared_ptr<IUVSurface>> surf;
	static void updateUVPoints();
	static void updatePoints();
	static void updateVectors();
public:
	static void Init();

	static void ShowTexture(unsigned int tex);
	static void ShowUVPoint(glm::fvec2 point);
	static void ShowUVSurface(std::shared_ptr<IUVSurface> s);
	static void ShowPoint(glm::fvec3 point);
	static void ShowVector(glm::fvec3 from, glm::fvec3 to);

	// Inherited via IGui
	virtual bool RenderGui() override;

	// Inherited via IRenderable
	virtual void Render(bool selected, VariableManager& vm) override;
};