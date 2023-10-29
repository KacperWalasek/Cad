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
	const float sizeMultiplier = 0.0125f;
	const int circleDivisions = 10;

	unsigned int VAO_rect, VAO_circ, 
				 VBO_rect, VBO_circ, 
				 EBO_rect, EBO_circ;

	std::vector<float> vert_rect,          vert_circ,
					   temporaryVert_rect, temporaryVert_circ;
	std::vector<int>   inds_rect,          inds_circ,
					   temporaryInds_rect, temporaryInds_circ;

	Shader rectShader, circShader;
	MillingPath path;
	int lastVisited = 0;
	
	float normalizeZ(float z) const;
	float stretchZ(float z) const;
	void flush();
	void addSegment(std::vector<float>& v_rect, std::vector<int>& i_rect,
					std::vector<float>& v_circ, std::vector<int>& i_circ,
					glm::fvec3 p1, glm::fvec3 p2, float r);
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