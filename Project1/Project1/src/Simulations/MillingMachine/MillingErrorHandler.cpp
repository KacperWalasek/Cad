#include "MillingErrorHandler.h"
#include <imgui/imgui.h>
#include <string>
#include "../../Debuger/Debuger.h"
#include "../../TextureCPUAccess.h"

void MillingErrorHandler::checkForErrors(TextureCPUAccess& cpuTex, const MillingPath& path, glm::fvec3 materialSize, int step)
{
	bool decreasesFlat = false;
	bool cutterHeightExceeded = false;
	for (int i = 0; i < sizeX; i++)
		for (int j = 0; j < sizeY; j++)
		{
			float dist = cpuTex.getXByIndex(i, j);
			if (dist == 0)
				continue;
			if (path.flat && path.positions[step].z != path.positions[step - 1].z)
				decreasesFlat = true;
			if (dist * materialSize.y > cutterHeight) // TODO
				cutterHeightExceeded = true;
		}

	if (path.positions[step].z < baseHeight)
		messages.push_back("Problem w kroku " + std::to_string(step) + " - frezowanie podstawki");
	if (cutterHeightExceeded)
		messages.push_back("Problem w kroku " + std::to_string(step) + " - frezowanie czescia nieskrawajaca");
	if (decreasesFlat)
		messages.push_back("Problem w kroku " + std::to_string(step) + " - frezowanie w do³ frezem plaskim");
}

void MillingErrorHandler::checkDescendingError(const MillingPath& path, glm::fvec3 materialSize)
{
	if(path.positions[0].z < materialSize[0])
	for (int i = 1; i < path.positions.size(); i++)
	{
		if (path.positions[i].z <  path.positions[i - 1].z &&
			fabsf(path.positions[i].x) / 10.0f <= materialSize.x / 2.0f + path.radius / 10.0f &&
			fabsf(path.positions[i].y) / 10.0f <= materialSize.y / 2.0f + path.radius / 10.0f)
			messages.push_back("Descending error in step " + std::to_string(i-1) + "-" + std::to_string(i));
	}
}

MillingErrorHandler::MillingErrorHandler(int sizeX, int sizeY, int baseHeight, int cutterHeight, std::vector<std::pair<std::string, MillingHeightMapRenderer>>& hms)
	: rectErrorShader(std::make_shared<Shader>(
		"Shaders/MillingPath/millingPath.vert",
		"Shaders/MillingPath/errorRect.frag")), 
	circErrorShader(std::make_shared<Shader>(
		"Shaders/MillingPath/millingPathCirc.vert",
		"Shaders/MillingPath/errorCirc.frag" )),
	lastStable(sizeX, sizeY, 1, true),
	errorTexture(sizeX, sizeY, 1, false),
	sizeX(sizeX), sizeY(sizeY), baseHeight(baseHeight), 
	cutterHeight(cutterHeight), hms(hms)
{
	circErrorShader->Init();
	rectErrorShader->Init();
	lastStable.Clear({ 1,0,0,1 });
	errorTexture.Clear({ 0,0,0,1 });
	Debuger::ShowTexture(lastStable.getTextureId());
	Debuger::ShowTexture(errorTexture.getTextureId());
}
 
std::vector<std::string> MillingErrorHandler::getMessages() const
{
	return messages;
}

void MillingErrorHandler::SetTextureSize(int sizeX, int sizeY)
{
	lastStable = TextureRenderer(sizeX, sizeY, 1, true);
	errorTexture = TextureRenderer(sizeX, sizeY, 1, false);
	lastStable.Clear({ 1,0,0,1 });
	errorTexture.Clear({ 0,0,0,1 });
}

void MillingErrorHandler::SetBaseHeight(int baseHeight)
{
	this->baseHeight = baseHeight;
}

void MillingErrorHandler::SetCutterHeight(int cutterHeight)
{
	this->cutterHeight = cutterHeight;
}

void MillingErrorHandler::UpdateLastStable(glm::fvec3 materialSize)
{
	if (hms.empty())
		return;
	float multiplier = 2.0f / materialSize.x;
	VariableManager vm;
	vm.AddVariable("color", glm::fvec4{ 1.0f,0.0f,0.0f,1.0f });
	vm.AddVariable("radius", 1);
	vm.AddVariable("flatMilling", true);
	vm.AddVariable("tex", 0);
	vm.AddVariable("treshold", 0.1f);
	vm.AddVariable("stepIndex", 0);

	lastStable.Clear({ 1,0,0,1 });
	for (int i = 0; i < hms.size(); i++)
	{
		hms[i].second.Flush(false);
		lastStable.Render(hms[i].second, vm);
		hms[i].second.Flush(true);
	}
}

void MillingErrorHandler::Validate(MillingHeightMapRenderer& hm, glm::fvec3 materialSize, int from, int to)
{
	const MillingPath& path = hm.GetPath();
	float multiplier = 2.0f / materialSize.x; // TODO
	VariableManager vm;
	vm.AddVariable("color", glm::fvec4{ 1.0f,0.0f,0.0f,1.0f });
	vm.AddVariable("radius", path.radius / materialSize.y);
	vm.AddVariable("flatMilling", path.flat);
	vm.AddVariable("tex", 0);
	vm.AddVariable("treshold", 0.1f);
	vm.AddVariable("stepIndex", 0);
	vm.AddVariable("stepCount", (int)path.positions.size());

	TextureCPUAccess cpuTex(errorTexture.getTextureId(), sizeX, sizeY);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lastStable.getTextureId());
	hm.SetUseExternalShaders(rectErrorShader, circErrorShader);
	for (int i = from; i < to; i++)
	{
		vm.SetVariable("stepIndex", i); // CLEAN
		hm.SetRenderOneSegment(i);

		errorTexture.Clear({ 0,0,0,1 });
		errorTexture.Render(hm, vm);

	}
	cpuTex.Update();
	checkForErrors(cpuTex, path, materialSize,0);
	hm.SetRenderAll();
	hm.SetUseInternalShaders();
}
