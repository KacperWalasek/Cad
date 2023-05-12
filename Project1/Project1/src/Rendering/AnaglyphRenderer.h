#pragma once
#include "Renderer.h"
#include "Camera.h"
#include "AnaglyphCamera.h"

class AnaglyphRenderer : public IGui
{
	float eyeDistance;
	float planeDistace;

	AnaglyphCamera leftCamera, rightCamera;
	unsigned int fbLeft, fbRight, texLeft, texRight;

	unsigned int screenVBO, screenEBO, screenVAO;

	Shader screenShader;
	void UpdateCameras();
public:
	bool enabled;
	AnaglyphRenderer(Camera& camera);
	
	void Init();
	void Render(Renderer& renderer, Scene& scene, std::vector<std::shared_ptr<IGui>>& guis, Camera& camera);

	virtual bool RenderGui(std::vector<std::shared_ptr<ISceneTracker>>& trackers) override;
};