#include "MillingErrorHandler.h"
#include <imgui/imgui.h>

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
	return { std::to_string(tex) };
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
	messages.push_back("Path is ok");
}
