#pragma once
#include "../Simulations/MillingMachine/MillingPath.h"
#include "../Scene/Scene.h"

class MillingPathCreator 
{
	static const int textureSize = 1000;
	static const glm::fvec4 rect; //TODO move to gui
	static const glm::fvec2 zRange;
	static unsigned int renderTextureHeightMap(Scene& scene);
public:
	static MillingPath CreateRoughtingPath(Scene& scene);
	static MillingPath CreateBasePath(Scene& scene) { return MillingPath();  }
	static MillingPath CreateRoundingPath(Scene& scene) { return MillingPath(); }
	static MillingPath CreateDetailPath(Scene& scene) { return MillingPath(); }

};