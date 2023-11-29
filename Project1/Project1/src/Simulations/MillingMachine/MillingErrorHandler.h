#pragma once
#include "../../interfaces/IErrorHandler.h"
#include "MillingPath.h"
#include "MillingHeightMapRenderer.h"
#include "../../Rendering/TextureRenderer.h"
#include "../../TextureCPUAccess.h"

class MillingErrorHandler : public IErrorHandler 
{
	std::vector<std::string> messages; 
	std::shared_ptr<Shader> circErrorShader, rectErrorShader, rectMilledAreaShader, circMilledAreaShader;
	void checkCutterHeightExceededErrors(TextureCPUAccess& cutterHeightTex, const MillingPath& path, glm::fvec3 materialSize);
	void checkFlatDescendingError(TextureCPUAccess& milledAreaTex, const MillingPath& path, glm::fvec3 materialSize);
	void checkBaseMillingError(const MillingPath& path, int beginInd, int endInd);
	TextureRenderer lastStable, cutterHeightTexture, milledAreaTexture;
	int sizeX, sizeY, baseHeight, cutterHeight;
	std::vector<std::pair<std::string, MillingHeightMapRenderer>>& hms;
public:
	MillingErrorHandler(int sizeX, int sizeY, int baseHeight, int cutterHeight, std::vector<std::pair<std::string, MillingHeightMapRenderer>>& hms);

	virtual std::vector<std::string> getMessages() const override;

	void UpdateLastStable(glm::fvec3 materialSize);
	void Validate(MillingHeightMapRenderer& hm, glm::fvec3 materialSize, int from, int to);
	void SetTextureSize(int sizeX, int sizeY);
	void SetBaseHeight(int baseHeight);
	void SetCutterHeight(int cutterHeight);
};