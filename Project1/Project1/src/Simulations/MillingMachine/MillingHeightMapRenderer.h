#pragma once
#include "../../interfaces/IRenderable.h"
#include "../../Rendering/Shader.h"
#include "MillingPath.h"

enum class PathState {
	Off,
	Running,
	Finished
};

class MillingHeightMapRenderer : public IRenderable {
	const float sizeMultiplier = 0.01f;

	unsigned int VAO, VBO, EBO;
	std::vector<float> vert;
	std::vector<int> inds;
	std::vector<float> temporaryVert;
	std::vector<int> temporaryInds;
	Shader shader;
	MillingPath path;
	int lastVisited = 0;
	
	float normalizeZ(float z) const;
	float stretchZ(float z) const;
	void flush();
	void addSegment(std::vector<float>& vertices, std::vector<int>& indices, glm::fvec3 p1, glm::fvec3 p2, float r);
	void clearTemporary();

	std::tuple<int,int,glm::fvec3> getPosition(float distance);
	void addFixedSegment(glm::fvec3 p1, glm::fvec3 p2, float r);
	void addTemporarySegment(glm::fvec3 p1, glm::fvec3 p2, float r);
public:

	MillingHeightMapRenderer(MillingPath path);
	
	glm::fvec3 SetDistance(float distance);
	glm::fvec3 Finalize();
	float GetLength();
	PathState GetState() const;
	bool IsFlat() const;
	float GetRadius() const;

	void Clear();
	virtual void Render(bool selected, VariableManager& vm) override;
};