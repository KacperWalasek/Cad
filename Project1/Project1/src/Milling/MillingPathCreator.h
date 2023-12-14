#pragma once
#include "../Simulations/MillingMachine/MillingPath.h"
#include "../Scene/Scene.h"
#include "../Geometry/Curves/UVEnvelope.h"
#include "../interfaces/IUVSurface.h"

template<typename T>
using VectorOfReference = std::vector<std::reference_wrapper<T>>;

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

	static float targetToWorldSpace(float pos);
	static glm::fvec3 worldToTargetSpace(glm::fvec3 pos);
	static unsigned int renderSceneHeightMap(Scene& scene);
	static std::vector<glm::fvec3> sampleHeigthMap(unsigned int heightMap, float level, bool startTopLeft);
	static std::vector<std::pair<int, int>> findIntersections(UVCurve c1, UVCurve c2);
	static bool segmentIntersect(glm::fvec2 a, glm::fvec2 b, glm::fvec2 c, glm::fvec2 d);
	static UVEnvelope createEnvelope(std::shared_ptr<IUVSurface> base);
	static std::vector<std::pair<glm::fvec2, UVEnvelopeIterator>> intersectEnvelopeWithLine(UVEnvelope& envelope, float t, bool u);
	static std::shared_ptr<IUVSurface> findSurfaceByName(Scene& scene, std::string name);
	static std::vector<glm::fvec3> millUVSurface(std::shared_ptr<IUVSurface> s, UVEnvelope& envelope, bool borderTop = false, bool side = false, bool asUV = false, std::vector<std::pair<float, float>> keyDistMultipliers = { {0,0} }, float frequency = 0.001f);
	static std::vector<glm::fvec3> uvPathsToWorldPaths(std::shared_ptr<IUVSurface> s, std::vector<glm::fvec2> uvPositions, bool side);
	static std::vector<glm::fvec3> uvPathsToWorldPathsFlat(std::shared_ptr<IUVSurface> s, std::vector<glm::fvec2> uvPositions);
	static std::vector<glm::fvec3> getRoundingPositions(UVEnvelopeIterator& envelopeIt, std::shared_ptr<IUVSurface> surf, bool asUV = false);
	static std::vector<glm::fvec3> uvPathsTo3D(std::vector<glm::fvec2> uvPositions);
	static std::vector<glm::fvec3> mergePaths(VectorOfReference<std::vector<glm::fvec3>> paths);
	static std::vector<glm::fvec3> safeMove(glm::fvec3 from, glm::fvec3 to);
	static float getCurvatureBasedDist(std::shared_ptr<IUVSurface> s, glm::fvec2 p, std::vector<glm::fvec2>& positions, int i, float influence);
	static float getCurvatureBasedDist(std::shared_ptr<IUVSurface> s, glm::fvec2 p, bool u, float influence);
	static float realDistToUV(std::shared_ptr<IUVSurface> s, float d, glm::fvec2 lineStart, glm::fvec2 lineEnd);

	static float realDist(std::shared_ptr<IUVSurface> surf, glm::fvec2 fromUV, glm::fvec2 toUV);
	static float realAngleCos(std::shared_ptr<IUVSurface> surf, glm::fvec2 fromUV, glm::fvec2 toUV1, glm::fvec2 toUV2);
	static glm::fvec3 applyBase(glm::fvec3 pos);
	static std::vector<glm::fvec2> millBase(std::shared_ptr<IUVSurface> surf, UVEnvelope& envelope, float r, bool direction);
	static std::vector<glm::fvec2> skipUnneadedPoints(std::shared_ptr<IUVSurface> s, std::vector<glm::fvec2> points, float minD, bool useCurvature);
public:
	static MillingPath CreateRoughtingPath(Scene& scene);
	static MillingPath CreateBasePath(Scene& scene);
	static MillingPath CreateRoundingPath(Scene& scene);
	static MillingPath CreateDetailPath(Scene& scene);

	static float uDistMult, vDistMult;
	static float minDetailDist, maxDetailDist, frequencyMultipier, dencityMultipier, frequecnyCurvatureInfluence, dencityCurvatureInfluence;

};