#include "MillingErrorHandler.h"
#include <imgui/imgui.h>
#include <string>

void MillingErrorHandler::readErrorTexture(unsigned int errorTexture, int stepCount)
{
	
	std::vector<float> data(1200 * 1200 * 3); 
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, errorTexture);
	auto e = glGetError();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &data[0]);
	e = glGetError();

	float minValue = 1;
	for (int i = 0; i < 1200 * 1200 * 3; i++)
		if (data[i] != 0.0f && data[i]<minValue) {
			minValue = data[i];
		}
	if (minValue != 1)
		messages.push_back("error in step " + std::to_string((int)(minValue / (float)stepCount)));
}

MillingErrorHandler::MillingErrorHandler()
	: rectErrorShader(std::make_shared<Shader>(
		"Shaders/MillingPath/millingPath.vert",
		"Shaders/MillingPath/errorRect.frag")), 
	circErrorShader(std::make_shared<Shader>(
		"Shaders/MillingPath/millingPathCirc.vert",
		"Shaders/MillingPath/errorCirc.frag" ))
{
	circErrorShader->Init();
	rectErrorShader->Init();
}
 
std::vector<std::string> MillingErrorHandler::getMessages() const
{
	return messages;
}

void MillingErrorHandler::validate(TextureRenderer& currentTex, const MillingPath& path)
{
	VariableManager vm;
	vm.AddVariable("color", glm::fvec4{ 1.0f,0.0f,0.0f,1.0f });
	vm.AddVariable("radius", path.radius * 0.0125);
	vm.AddVariable("flatMilling", path.flat);
	vm.AddVariable("tex", 0);
	vm.AddVariable("treshold", 0.1f);
	vm.AddVariable("stepIndex", 0);
	vm.AddVariable("stepCount", (int)path.positions.size());

	MillingHeightMapRenderer hm(path);
	hm.Finalize();

	TextureRenderer errorTexRenderer(1200, 1200, 1, false);
	errorTexRenderer.Clear({0,0,0,1});
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, currentTex.getTextureId());
	for (int i = 1; i < path.positions.size(); i++)
	{
		vm.SetVariable("stepIndex", i);
		hm.SetRenderOneSegment(i);

		hm.SetUseExternalShaders(rectErrorShader, circErrorShader);
		errorTexRenderer.Render(hm, vm);

		hm.SetUseInternalShaders();
		currentTex.Render(hm, vm);
	}
	tex = errorTexRenderer.getTextureId();
	readErrorTexture(errorTexRenderer.getTextureId(), path.positions.size());
	messages.push_back("Path is ok");
}
