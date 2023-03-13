#pragma once
#include "Mesh.h"
#include <memory>

class SceneObject
{
public:
	SceneObject()
		:mesh(), gui(nullptr) {}
	SceneObject(Mesh mesh, std::shared_ptr<IGui> gui)
		:mesh(mesh), gui(gui) {}
	Mesh mesh;
	std::shared_ptr<IGui> gui;
};