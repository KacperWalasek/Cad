#pragma once
#include "../Simulations/MillingMachine/MillingPath.h"
#include "../Scene/Scene.h"

class MillingPathCreator 
{
	//TODO move all consts to gui
	static const int textureSize = 400;
	static const glm::fvec4 rect; 
	static const glm::fvec2 zRange;
	static const glm::fvec3 targetSize;
	static const float targetBaseHeight;
	static const float epsilon;
	static const float roughtPathsTranslation;

	static unsigned int renderSceneHeightMap(Scene& scene);
	static std::vector<glm::fvec3> sampleHeigthMap(unsigned int heightMap, float level, bool startTopLeft);
public:
	static MillingPath CreateRoughtingPath(Scene& scene);
	static MillingPath CreateBasePath(Scene& scene) { return MillingPath();  }
	static MillingPath CreateRoundingPath(Scene& scene) { return MillingPath(); }
	static MillingPath CreateDetailPath(Scene& scene) { return MillingPath(); }
};