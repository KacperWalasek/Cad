#pragma once
#include "EventHanlderInterfaces.h"
#include "../Scene/Scene.h"
#include "../Geometry/Surfaces/SurfaceC0.h"
#include "../Geometry/Surfaces/GregoryPatch.h"
#include <set>

class ObjectFactory : public IKeyEventHandler, public IGui
{
	Scene& scene;
	std::vector<Patch> getSelectedPatches();

	bool checkEdge(std::vector<Patch>& patches, Patch& p, int c1, int c2);
	bool isEdgeBorder(std::map<int, std::vector<std::pair<int,int>>>& border, int p, int c1, int c2);
	std::vector<Hole> findHoles(std::vector<Patch>& patches);

	void CreateFillPatch();
	void CreateIntersection();

	std::vector<Patch> surfaceToPatches(std::shared_ptr<SurfaceC0> surface);
	bool isHoleBlinded(Hole& hole);

	float intersectionStep;
	bool cursor;
public:
	ObjectFactory(Scene& scene);

	virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;

	// Inherited via IGui
	virtual bool RenderGui() override;
};