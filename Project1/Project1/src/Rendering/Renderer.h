#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include "Window.h"
#include "../interfaces/IGui.h"
#include "Camera.h"
#include "../interfaces/ISceneElement.h"
#include "../Scene/Scene.h"
#include "VariableManager.h"

class Renderer
{
	Window& window;
	Shader shader;
	bool isMoving = false;
public:
	VariableManager variableManager;
	Renderer(Window& window);
	void Init(); 

	void BeginRender(Camera& camera);
	void RenderScene(Camera& camera, Scene& scene);
	void RenderGui(Scene& scene, std::vector<std::shared_ptr<IGui>>& guis);
	void Finalize(Scene& scene);
};