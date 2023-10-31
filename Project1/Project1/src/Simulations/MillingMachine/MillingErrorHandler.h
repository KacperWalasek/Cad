#pragma once
#include "../../interfaces/IErrorHandler.h"
#include "MillingPath.h"
#include "MillingHeightMapRenderer.h"
#include "../../Rendering/TextureRenderer.h"

class MillingErrorHandler : public IErrorHandler 
{
	std::vector<std::string> messages;
	unsigned int tex;
	std::shared_ptr<Shader> circErrorShader, rectErrorShader;
	void readErrorTexture(unsigned int errorTexture, int stepCount);
public:
	MillingErrorHandler();

	virtual std::vector<std::string> getMessages() const override;

	void validate(TextureRenderer& currentTex, const MillingPath& path, glm::fvec3 materialSize, float baseHeight);
};