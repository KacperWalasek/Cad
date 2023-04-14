#include "CurveC0.h"
#include <imgui/imgui_stdlib.h>
#include "Scene.h"
#include <glm/gtc/type_ptr.hpp>

Indexer CurveC0::indexer;

CurveC0::CurveC0(Camera& camera)
	: CurveC0(camera, {})
{
}

CurveC0::CurveC0(Camera& camera, std::vector<std::shared_ptr<Point>> points)
	: camera(camera), points(points), name("CurveC0-" + std::to_string(indexer.getNewIndex())), addSelected(false),
	removeSelected(false), showChain(false),
	shader("Shaders/test.vert", "Shaders/fragmentShader.frag")
{
	shader.Init();
	shader.loadShaderFile("Shaders/test.tesc", GL_TESS_CONTROL_SHADER);
	shader.loadShaderFile("Shaders/test.tese", GL_TESS_EVALUATION_SHADER);

	UpdateMeshes();
}

std::string CurveC0::getName() const
{
	return name;
}

void CurveC0::Render(bool selected)
{
	if (showChain)
		for (auto& b : beziers)
			b.chainMesh.Render();
	shader.use();
	
	unsigned int colorLoc = glGetUniformLocation(shader.ID, "color");
	if (selected)
		glUniform4f(colorLoc, 1.0f, 0.5f, 0.0f, 1.0f);
	else
		glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
	unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
	glm::fmat4x4 centerMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(centerMatrix));
	for (auto& b : beziers)
		b.Render(shader);
}

void CurveC0::RenderGui()
{
	ImGui::Begin("C0 Curve");
	ImGui::InputText("Name", &name);

	if (ImGui::Checkbox("Add on select", &addSelected))
		if (addSelected)
			removeSelected = false;
	if (ImGui::Checkbox("Remove on select", &removeSelected))
		if (removeSelected)
			addSelected = false;

	ImGui::Checkbox("Show chain", &showChain);
	ImGui::End();
}

void CurveC0::UpdateMeshes()
{
	beziers.clear();
	int bezierCount = ceil((points.size() - 1) / 3.0f);

	std::vector<glm::fvec4> p;
	for(int i = 0; i < bezierCount; i++)
	{
		p.clear();
		for (int j = 0; j < 4; j++)
		{
			if (i * 3 + j >= points.size())
				break;
			p.push_back(points[i*3+j]->getTransform().location);
		}
		beziers.emplace_back();
		beziers.rbegin()->UpdateMeshes(p);
	}
}


void CurveC0::onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	auto curveAsSceneElem = std::find_if(scene.objects.begin(), scene.objects.end(),
		[this](const std::pair<std::shared_ptr<ISceneElement>, bool>& obj)
		{
			return obj.first.get() == this;
		});
	if (curveAsSceneElem != scene.objects.end() && curveAsSceneElem->second)
	{
		std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);
		if (p)
		{
			points.push_back(p);
			UpdateMeshes();
		}
	}
}

void CurveC0::onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);

	auto it = std::find(points.begin(), points.end(), elem);
	if (it != points.end())
	{
		points.erase(it);
		UpdateMeshes();
	}

}

void CurveC0::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);
	if (p)
	{
		auto pointIt = std::find(points.begin(), points.end(), elem);
		if (pointIt == points.end())
		{
			if (addSelected)
			{
				std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);
				if (p)
				{
					points.push_back(p);
					UpdateMeshes();
				}
			}
		}
		else
			if (removeSelected)
			{
				points.erase(pointIt);
				UpdateMeshes();
			}
		if (removeSelected || addSelected)
		{
			auto curveAsSceneElem = std::find_if(scene.objects.begin(), scene.objects.end(),
				[this](const std::pair<std::shared_ptr<ISceneElement>, bool>& obj)
				{
					return obj.first.get() == this;
				});
			scene.Select(curveAsSceneElem->first);
		}
	}
}

void CurveC0::onMove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	if (std::find(points.begin(), points.end(), elem) != points.end())
		UpdateMeshes();
}
