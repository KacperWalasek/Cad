#pragma once
#include "../Simulations/MillingMachine/MillingPath.h"
#include "../Scene/Scene.h"
#include "../Geometry/Curves/UVEnvelope.h"
#include "../interfaces/IUVSurface.h"

enum class Direction {
	UpRight,

};
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

	static void intersect(Scene& scene, std::shared_ptr<IUVSurface> s1, std::shared_ptr<IUVSurface> s2, glm::fvec3 cursorPos);

	static glm::fvec3 worldToTargetSpace(glm::fvec3 pos);
	static unsigned int renderSceneHeightMap(Scene& scene);
	static std::vector<glm::fvec3> sampleHeigthMap(unsigned int heightMap, float level, bool startTopLeft);
	static std::vector<std::pair<int, int>> findIntersections(UVCurve c1, UVCurve c2);
	static bool segmentIntersect(glm::fvec2 a, glm::fvec2 b, glm::fvec2 c, glm::fvec2 d);
	static UVEnvelope createEnvelope(std::shared_ptr<IUVSurface> base);
	static std::vector<UVEnvelopeIterator> intersectEnvelopeWithLine(UVEnvelope& envelope, float t, bool u);
	static std::shared_ptr<IUVSurface> findSurfaceByName(Scene& scene, std::string name);
	static std::vector<glm::fvec3> millUVSurface(std::shared_ptr<IUVSurface> s, UVEnvelope& envelope, Direction direction, bool borderTop = false, bool side = false);
	static std::vector<glm::fvec3> uvPathsToWorldPaths(std::shared_ptr<IUVSurface> s, std::vector<glm::fvec2> uvPositions, bool side);
public:
	static MillingPath CreateRoughtingPath(Scene& scene);
	static MillingPath CreateBasePath(Scene& scene);
	static MillingPath CreateRoundingPath(Scene& scene);
	static MillingPath CreateDetailPath(Scene& scene);
};