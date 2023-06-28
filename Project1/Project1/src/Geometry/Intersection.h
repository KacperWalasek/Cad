#pragma once
#include <memory>
#include "../interfaces/IUVSurface.h"
#include "../interfaces/ISceneElement.h"
#include "../interfaces/IRenderable.h"
#include "../interfaces/IGui.h"
#include "../Indexer.h"
#include "../UI/SceneVector.h"
#include "BrokenLine.h"

class Intersection : public ISceneElement, public IRenderable, public IGui
{
	const float step = 0.1f;
	const float eps = 1e-4;
	const float maxAlpha = 10000.0f;
	static Indexer indexer;

	std::string name;

	Shader uvShader;
	unsigned int VAO, VBO, EBO;
	unsigned int uvS1Fb, uvS2Fb;
	
	glm::fvec4 wrap(glm::fvec4 x) const;
	float distSqr(glm::fvec4 x);
	glm::fvec4 distGrad(glm::fvec4 x);
	float findAlpha(glm::fvec4 x, glm::fvec4 d);
	float findAlphaRange(glm::fvec4 x, glm::fvec4 d);
	glm::fvec4 findIntersectionPoint();
	void findIntersection(glm::fvec4 x);
	std::vector<glm::fvec4> findIntersectionPointsInDirection(glm::fvec4 x, bool dir);
	glm::mat4x4 jacoby(glm::fvec4 x, glm::fvec3 d) const;

	std::vector<std::unique_ptr<SceneVector>> debugVec;

	void floodFill(std::vector<GLbyte>& pixels, glm::ivec2 size, glm::ivec3 color, glm::ivec2 start, glm::bvec2 wrapped);
	int pixelsInds(int x, int y);

	BrokenLine sceneCurve, s1UVLine, s2UVLine;
public:
	std::shared_ptr<IUVSurface> s1, s2;
	unsigned int uvS1Tex, uvS2Tex;
	bool reverse, intersect;

	Intersection(std::shared_ptr<IUVSurface> s1, std::shared_ptr<IUVSurface> s2);


	// Inherited via ISceneElement
	virtual std::string getName() const override;


	// Inherited via IRenderable
	virtual void Render(bool selected, VariableManager& vm) override;
	virtual bool RenderGui() override;

};