#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include "Window.h"
#include "Mesh.h"
#include "../interfaces/IGui.h"
#include "Camera.h"
#include "SceneObject.h"
#include "../Scene/ISceneElement.h"
#include "../Scene/Scene.h"
#include "VariableManager.h"

class Renderer
{
	Window& window;
	Shader shader;
	VariableManager variableManager;
	bool isMoving = false;
public:
	Renderer(Window& window);
	void Init(); 
	void Update(Camera& camera);
	void Render(Camera& camera, Scene& scene, std::vector<std::shared_ptr<IGui>>& guis);
};