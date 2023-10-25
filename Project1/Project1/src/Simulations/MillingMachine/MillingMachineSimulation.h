#pragma once
#include "../ISimulation.h"
#include "../../interfaces/IGui.h"
#include "../../interfaces/ISceneElement.h"
#include "../../interfaces/IRenderable.h"
#include <glm/gtc/type_precision.hpp>
#include "MillingPath.h"
#include "Cutter.h"
#include "MaterialCube.h"
#include "MillingHeightMapRenderer.h"

class MillingMachineSimulation 
	: public ISimulation, public IGui, public ISceneElement, public IRenderable 
{
	const float sizeMultiplier = 0.01f;

	MillingHeightMapRenderer hm;
	std::shared_ptr<TextureRenderer> renderer;

	// Visualisation
	Cutter cutter;
	MaterialCube materialCube = {600,600};
	
	// State
	bool running = false;
	float timePassed = 0;
	MillingPath path;
	ProceduralTexture tex = { 600,600 };
	int lastVisited = 0;

	// Material
	glm::ivec2 divisions = { 600,600 };
	glm::ivec3 materialSize = { 15,5,15 };
	int baseHeight = 15;

	// Simulation
	int speed = 10;
	bool instant = false;
	bool finished = false;
	
	// Cutter
	int cutterSize = 1;
	int height = 4;
	bool flat = false;
	std::tuple<int, int, glm::fvec3> getPosition(float t);
	void applyStep(glm::fvec3 p1, glm::fvec3 p2);
	void renderInstant();
public:
	MillingMachineSimulation();

	virtual void start() override;
	virtual void stop() override;
	virtual void reset() override;
	virtual void update(float dt) override;
	virtual bool isRunning() const override;

	// Inherited via IGui
	virtual bool RenderGui() override;

	// Inherited via ISceneElement
	virtual std::string getName() const override;

	// Inherited via IRenderable
	virtual void Render(bool selected, VariableManager& vm) override;
};