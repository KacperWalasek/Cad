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
#include "MillingErrorHandler.h"
#include "MillingPathVisualizer.h"

class MillingMachineSimulation 
	: public ISimulation, public IGui, public ISceneElement, public IRenderable 
{
	
	std::vector<std::pair<std::string, MillingHeightMapRenderer>> hms;
	int selectedHM = 0;

	std::shared_ptr<MillingErrorHandler> errorHandler;
	std::shared_ptr<TextureRenderer> renderer;

	// Visualisation
	Cutter cutter;
	MaterialCube materialCube = {1200,1200 };
	MillingPathVisualizer millingPathVisualizer;
	
	// State
	bool running = false;
	float passedPathLength = 0.0f;
	MillingPath path;
	ProceduralTexture tex = { 1200,1200 };
	int lastVisited = 0;

	// Material
	glm::ivec2 divisions = { 1200,1200 };
	glm::ivec3 materialSize = { 15,5,15 };
	int baseHeight = 5;

	// Simulation
	float speed = 0.02f;
	bool instant = false;
	bool finished = false;
	bool showPaths = false;
	
	// Cutter
	int cutterSize = 1;
	int height = 4;
	bool flat = false;
	std::tuple<int, int, glm::fvec3> getPosition(float t);
	void applyStep(glm::fvec3 p1, glm::fvec3 p2);
	void renderInstant();
	void handleErrors();
public:
	MillingMachineSimulation();

	virtual void start() override;
	virtual void stop() override;
	virtual void reset() override;
	virtual void update(float dt) override;
	virtual bool isRunning() const override;

	std::shared_ptr<MillingErrorHandler> GetErrorHandler();

	// Inherited via IGui
	virtual bool RenderGui() override;

	// Inherited via ISceneElement
	virtual std::string getName() const override;

	// Inherited via IRenderable
	virtual void Render(bool selected, VariableManager& vm) override;
};