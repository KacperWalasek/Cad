#include "MillingErrorHandler.h"
#include <imgui/imgui.h>
#include <string>
#include "../../Debuger/Debuger.h"
#include "../../TextureCPUAccess.h"
#include <set>

void MillingErrorHandler::checkCutterHeightExceededErrors(TextureCPUAccess& cutterHeightTex, const MillingPath& path, glm::fvec3 materialSize)
{
	int stepCount = path.positions.size();
	std::set<int> cutterHeightExceeded;
	for (int i = 0; i < sizeX; i++)
		for (int j = 0; j < sizeY; j++)
		{
			float dist = cutterHeightTex.getXByIndex(i, j);
			if (dist != 0)
				cutterHeightExceeded.insert(stepCount * dist);
		}

	for (int i : cutterHeightExceeded)
		messages.push_back("Problem w kroku " + std::to_string(i) + " - frezowanie czescia nieskrawajaca");
}

void MillingErrorHandler::checkFlatDescendingError(TextureCPUAccess& milledAreaTex, const MillingPath& path, glm::fvec3 materialSize)
{
	int stepCount = path.positions.size();
	std::set<int> decreasesFlat;
	for (int i = 0; i < sizeX; i++)
		for (int j = 0; j < sizeY; j++)
		{
			float dist = milledAreaTex.getXByIndex(i, j);
			if (dist == 0)
				continue;
			int ind = stepCount * dist;
			if (path.flat && path.positions[ind+1].z < path.positions[ind].z)
				decreasesFlat.insert(ind);
		}
	for (int i : decreasesFlat)
		messages.push_back("Problem w kroku " + std::to_string(i) + " - frezowanie w dol frezem plaskim");
}

void MillingErrorHandler::checkBaseMillingError(const MillingPath& path, int beginInd, int endInd)
{
	for (int i = beginInd; i<endInd; i++)
		if (path.positions[i].z < baseHeight)
			messages.push_back("Problem w kroku " + std::to_string(i) + " - frezowanie podstawki");
}

MillingErrorHandler::MillingErrorHandler(int sizeX, int sizeY, int baseHeight, int cutterHeight, std::vector<std::pair<std::string, MillingHeightMapRenderer>>& hms)
	: rectErrorShader(std::make_shared<Shader>(
		"Shaders/MillingPath/millingPath.vert",
		"Shaders/MillingPath/errorRect.frag")), 
	circErrorShader(std::make_shared<Shader>(
		"Shaders/MillingPath/millingPathCirc.vert",
		"Shaders/MillingPath/errorCirc.frag" )),
	rectMilledAreaShader(std::make_shared<Shader>(
		"Shaders/MillingPath/millingPath.vert",
		"Shaders/MillingPath/milledAreaRect.frag")),
	circMilledAreaShader(std::make_shared<Shader>(
		"Shaders/MillingPath/millingPathCirc.vert",
		"Shaders/MillingPath/milledAreaCirc.frag")),
	lastStable(sizeX, sizeY, 1, true),
	cutterHeightTexture(sizeX, sizeY, 1, false),
	milledAreaTexture(sizeX, sizeY, 1, false),
	sizeX(sizeX), sizeY(sizeY), baseHeight(baseHeight), 
	cutterHeight(cutterHeight), hms(hms)
{
	circErrorShader->Init();
	rectErrorShader->Init();
	rectMilledAreaShader->Init();
	circMilledAreaShader->Init();
	lastStable.Clear({ 1,0,0,1 });
	cutterHeightTexture.Clear({ 0,0,0,1 });
	milledAreaTexture.Clear({ 0,0,0,1 });
	Debuger::ShowTexture(lastStable.getTextureId());
	Debuger::ShowTexture(cutterHeightTexture.getTextureId());
	Debuger::ShowTexture(milledAreaTexture.getTextureId());
}
 
std::vector<std::string> MillingErrorHandler::getMessages() const
{
	return messages;
}

void MillingErrorHandler::SetTextureSize(int sizeX, int sizeY)
{
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	lastStable = TextureRenderer(sizeX, sizeY, 1, true);
	cutterHeightTexture = TextureRenderer(sizeX, sizeY, 1, false);
	milledAreaTexture = TextureRenderer(sizeX, sizeY, 1, false);
	lastStable.Clear({ 1,0,0,1 });
	cutterHeightTexture.Clear({ 0,0,0,1 });
	milledAreaTexture.Clear({ 0,0,0,1 });
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
	float multiplier = 2.0f / materialSize.x;
	VariableManager vm;
	vm.AddVariable("color", glm::fvec4{ 1.0f,0.0f,0.0f,1.0f });
	vm.AddVariable("radius", path.radius / materialSize.y);
	vm.AddVariable("flatMilling", path.flat);
	vm.AddVariable("tex", 0);
	vm.AddVariable("treshold", cutterHeight/materialSize.y);
	vm.AddVariable("stepIndex", 0);
	vm.AddVariable("stepCount", (int)path.positions.size());

	TextureCPUAccess cutterHeightTex(cutterHeightTexture.getTextureId(), sizeX, sizeY);
	TextureCPUAccess milledAreaTex(milledAreaTexture.getTextureId(), sizeX, sizeY);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lastStable.getTextureId());
	cutterHeightTexture.Clear({ 0,0,0,1 });
	milledAreaTexture.Clear({ 0,0,0,1 });
	for (int i = from; i < to; i++)
	{
		vm.SetVariable("stepIndex", i);
		hm.SetRenderOneSegment(i);

		hm.SetUseExternalShaders(rectErrorShader, circErrorShader);
		cutterHeightTexture.Render(hm, vm);

		hm.SetUseExternalShaders(rectMilledAreaShader, circMilledAreaShader);
		milledAreaTexture.Render(hm, vm);

		hm.SetUseInternalShaders();
		lastStable.Render(hm, vm);

	}
	hm.SetRenderAll();
	hm.SetUseInternalShaders();

	cutterHeightTex.Update();
	milledAreaTex.Update();
	checkCutterHeightExceededErrors(cutterHeightTex, path, materialSize);
	checkFlatDescendingError(milledAreaTex, path, materialSize);
	checkBaseMillingError(path, from, to);
}
