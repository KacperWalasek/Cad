#include "MillingPathCreator.h"
#include "../Rendering/TextureRenderer.h"
#include "SceneHeightMapRenderer.h"
#include "../Debuger/Debuger.h"
#include <numbers>

const glm::fvec4 MillingPathCreator::rect = { -25, 25,10,-40 };
const glm::fvec2 MillingPathCreator::zRange = { 0,-10 };
const glm::fvec3 MillingPathCreator::targetSize = { 15.0f, 5.0f, 15.0f };
const float MillingPathCreator::targetBaseHeight = 5.0f;
const float MillingPathCreator::epsilon = 0.001f;
const float MillingPathCreator::roughtPathsTranslation = 0.5f;


unsigned int MillingPathCreator::renderSceneHeightMap(Scene& scene)
{
	SceneHeightMapRenderer sceneRenderer(scene);
	TextureRenderer tr(textureSize, textureSize, 3, true);

	float rangeLen = zRange.y - zRange.x;

	// setup uniforms
	VariableManager vm;
	vm.AddVariable("projMtx", glm::ortho<float>(rect.x,rect.y,rect.z, rect.w, 0.001f, fabsf(rangeLen)*2));
	vm.AddVariable("viewMtx", glm::translate(glm::rotate<float>(glm::identity<glm::fmat4x4>(), std::numbers::pi, { 0,1,0 }), { 0,0,-rangeLen }));
	vm.AddVariable("modelMtx", glm::identity<glm::fmat4x4>());
	vm.AddVariable("color", glm::fvec4(0,1,1,1));
	vm.AddVariable("divisionV", 4);
	vm.AddVariable("divisionU", 4);
	vm.AddVariable("heightRange", zRange);

	// render
	tr.Clear({ 0,0,0,0 });
	tr.Render(sceneRenderer, vm);
	
	return tr.getTextureId();
}
std::vector<glm::fvec3> MillingPathCreator::sampleHeigthMap(unsigned int heightMap, float level, bool startTopLeft)
{
	float cadWidth = rect.y - rect.x;
	float cadHeight = fabsf(zRange.x - zRange.y);
	float materialLimitInCadSpace = targetSize.z * cadWidth / targetSize.x;
	float materialLimitInTextureSpace = materialLimitInCadSpace / cadHeight;
	float levelInTextureSpace = level * materialLimitInTextureSpace;
	float levelInTargetSpace = level * targetSize.y;

	std::vector<float> data(textureSize * textureSize * 3);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, heightMap);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &data[0]);
	
	std::vector<glm::fvec3> positions;
	positions.reserve(40 * 40);
	for (int i = 0; i < textureSize; i += 10)
	{
		for (int j = 0; j < textureSize; j += 10)
		{
			int k = (i / 10) % 2 == 0 ? j : textureSize - j;
			glm::fvec3 positionTargetSpace = {
				i / (float)textureSize * targetSize.x - targetSize.x / 2.0f,
				k / (float)textureSize * targetSize.z - targetSize.z / 2.0f,
				0 };
			float heightTexSpace = data[(i * textureSize + k) * 3];
			float heightTargetSpace = targetSize.y * heightTexSpace / materialLimitInTextureSpace;
			positionTargetSpace.z = heightTexSpace < levelInTextureSpace ? levelInTargetSpace : heightTargetSpace;
			/*if (j == 0) {
				glm::fvec3 highPos = positionTargetSpace;
				highPos.z = (targetSize.y + 1)*10.0f;
				positions.push_back(highPos);
			}*/
			if (j == 0 || positionTargetSpace.y != positions.rbegin()->y)
				positions.push_back((positionTargetSpace + glm::fvec3(0.0f, 0.0f, targetBaseHeight)) * 10.0f);
		}
/*
		glm::fvec3 highPos = *positions.rbegin();
		highPos.z = (targetSize.y + 1) * 10.0f;
		positions.push_back(highPos);*/
	}
	if (!startTopLeft)  std::reverse(positions.begin(), positions.end());
	return positions;
}

MillingPath MillingPathCreator::CreateRoughtingPath(Scene& scene)
{
	unsigned int tex = renderSceneHeightMap(scene);
	Debuger::ShowTexture(tex);

	std::vector<glm::fvec3> positions = sampleHeigthMap(tex, 0.5f, true);
	std::vector<glm::fvec3> positions0 = sampleHeigthMap(tex, 0.0f, false);
	positions.insert(positions.end(), positions0.begin(), positions0.end());
	return MillingPath(positions, 12, false);
}
