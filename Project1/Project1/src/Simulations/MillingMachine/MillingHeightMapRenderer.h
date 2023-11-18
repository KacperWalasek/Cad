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

	const int circleDivisions = 10;

	unsigned int VAO_rect, VAO_circ, 
				 VBO_rect, VBO_circ, 
				 EBO_rect, EBO_circ;
	int rectIndCount, circIndCount;

	std::vector<float> vert_rect,          vert_circ,
					   temporaryVert_rect, temporaryVert_circ;
	std::vector<int>   inds_rect,          inds_circ,
					   temporaryInds_rect, temporaryInds_circ;

	Shader rectShader, circShader;
	MillingPath path;
	glm::fvec3 materialSize; 
	glm::fvec3 sizeMultiplier;

	// Modes
	bool renderOneSegment = false;
	bool useExternalShader = false;
	int segmentToRender = 0;
	std::shared_ptr<Shader> externalShaderRect, externalShaderCirc;


	float stretchZ(float z) const;
	void addSegment(std::vector<float>& v_rect, std::vector<int>& i_rect,
					std::vector<float>& v_circ, std::vector<int>& i_circ,
					glm::fvec3 p1, glm::fvec3 p2, float r);
	void clearTemporary();

	std::tuple<int,int,glm::fvec3> getPosition(float distance);
	void addFixedSegment(glm::fvec3 p1, glm::fvec3 p2, float r);
	void addTemporarySegment(glm::fvec3 p1, glm::fvec3 p2, float r);

	void renderAll(VariableManager& vm);
	void renderSegment(VariableManager& vm);
public:
	MillingHeightMapRenderer(MillingPath path, glm::fvec3 materialSize);
	void SetMaterialSize(glm::fvec3 materialSize);

	int lastVisited = 0;
	glm::fvec3 SetDistance(float distance);
	glm::fvec3 Finalize();
	bool MakeNextStep();

	float GetLength();
	PathState GetState() const;
	bool IsFlat() const;
	float GetRadius() const;
	const MillingPath& GetPath() const;

	void Clear();
	virtual void Render(bool selected, VariableManager& vm) override;

	void SetRenderOneSegment(int i);
	void SetRenderAll();

	void SetUseExternalShaders(std::shared_ptr<Shader> externalShaderRect, std::shared_ptr<Shader> externalShaderCirc);
	void SetUseInternalShaders();

	void Flush(bool useTmp);
};