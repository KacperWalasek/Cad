#pragma once
#include <memory>
#include "../interfaces/IUVSurface.h"
#include "../interfaces/ISceneElement.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/IGui.h"
#include "../Indexer.h"
#include "../UI/SceneVector.h"
#include "BrokenLine.h"
#include <functional>
#include "../interfaces/ISceneModifier.h"
#include "Curves/InterpolationCurve.h"
#include "Curves/UVEnvelope.h"

class Intersection : public ISceneElement, public IRenderable, public IGui, public ISceneModifier, public ISceneTracker
{
	float step;
	const float eps = 1e-4;
	const float maxAlpha = 10000.0f;
	static Indexer indexer;

	std::string name;

	Shader uvShader;
	unsigned int uvS1Fb, uvS2Fb;
	
	glm::fvec4 wrap(glm::fvec4 x) const;
	int pixelsInds(int x, int y);
	float findMaxAlpha(glm::fvec4 x, glm::fvec4 dir, bool wrap = true);

	float distSqr(glm::fvec4 x);
	glm::fvec4 distGrad(glm::fvec4 x);

	glm::fvec4 bestSample(int division);
	
	glm::fvec4 closestToCursor(glm::fvec3 cursorPosition, int division);
	glm::fvec4 bestSampleCursor(glm::fvec3 cursorPosition, int division);
	float cursorDist(glm::fvec4 x,glm::fvec3 cursorPosition);
	glm::fvec4 cursorGrad(glm::fvec4 x, glm::fvec3 cursorPosition);


	float findAlpha(glm::fvec4 x, glm::fvec4 d, std::function<float(glm::fvec4)> val, std::function<glm::fvec4(glm::fvec4)> grad);
	float findAlphaRange(glm::fvec4 x, glm::fvec4 d, std::function<float(glm::fvec4)> val);

	glm::fvec4 findIntersectionPoint(glm::fvec4 beginPoint);
	void extendIntersection(glm::fvec4 x);
	
	std::tuple<std::vector<glm::fvec4>, bool> findIntersectionPointsInDirection(glm::fvec4 x, bool dir);
	glm::mat4x4 jacoby(glm::fvec4 x, glm::fvec3 d) const;

	std::vector<std::unique_ptr<SceneVector>> debugVec;

	void floodFill(std::vector<GLbyte>& pixels, glm::ivec2 size, glm::ivec3 color, glm::ivec2 start, glm::bvec2 wrapped);

	void initTextures();
	void renderUVTexture(std::shared_ptr<IUVSurface> s, unsigned int uvFb, unsigned int uvTex, BrokenLine& line);

	BrokenLine sceneCurve, s1UVLine, s2UVLine;
	std::vector<std::shared_ptr<ISceneElement>> toAdd;
public:
	bool valid;
	std::shared_ptr<IUVSurface> s1, s2;
	unsigned int uvS1Tex, uvS2Tex;

	Intersection(std::shared_ptr<IUVSurface> s1, std::shared_ptr<IUVSurface> s2, bool cursor, glm::fvec3 cursorPos, float step);

	// Inherited via ISceneElement
	virtual std::string getName() const override;

	// Inherited via ISceneTracker
	virtual void onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onMove(Scene& scene, std::shared_ptr<ISceneElement> elem) override;
	virtual void onCollapse(Scene& scene, std::vector<std::shared_ptr<Point>>& collapsed, std::shared_ptr<Point> result) override;


	// Inherited via IRenderable
	virtual void Render(bool selected, VariableManager& vm) override;
	virtual bool RenderGui() override;


	virtual std::vector<std::shared_ptr<ISceneElement>> GetAddedObjects() override;
	virtual std::vector<std::shared_ptr<ISceneElement>> GetRemovedObjects() override;

	UVCurve GetUVCurve(std::shared_ptr<IUVSurface> surface) const;
	std::shared_ptr<IUVSurface> GetOtherSurface(std::shared_ptr<IUVSurface> thisSurf) const;
	unsigned int GetTextureForSurface(std::shared_ptr<IUVSurface> surface) const;
};