#include "CurveC2.h"
#include "Scene.h"
#include <imgui/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>
CurveC2::CurveC2(Camera& camera, std::vector<std::shared_ptr<Point>> points)
	: camera(camera), points(points), name("Curve"), addSelected(false),
	removeSelected(false),
	shader("Shaders/test.vert", "Shaders/fragmentShader.frag"),
	deBoorShader("Shaders/DeBoor/deboor.vert", "Shaders/fragmentShader.frag")
{
	shader.Init();
	shader.loadShaderFile("Shaders/test.tesc", GL_TESS_CONTROL_SHADER);
	shader.loadShaderFile("Shaders/test.tese", GL_TESS_EVALUATION_SHADER);

	deBoorShader.Init();
	deBoorShader.loadShaderFile("Shaders/DeBoor/deboor.tesc", GL_TESS_CONTROL_SHADER);
	deBoorShader.loadShaderFile("Shaders/DeBoor/deboor.tese", GL_TESS_EVALUATION_SHADER);

	UpdateMeshes();

}

void CurveC2::UpdateMeshes()
{
	UpdateBeziers();
	chainMesh.vertices.clear();
	chainMesh.indices.clear();

	for (int i = 0; i<points.size(); i++)
	{
		glm::fvec4 v = points[i]->getTransform().location;
		chainMesh.vertices.push_back(v.x);
		chainMesh.vertices.push_back(v.y);
		chainMesh.vertices.push_back(v.z);

		if (i != 0)
		{
			chainMesh.indices.push_back(i - 1);
			chainMesh.indices.push_back(i);
		}
	}
	chainMesh.Update();
}

void CurveC2::UpdateBeziers()
{
	beziers.clear();
	for (int i = 1; i < (int)(points.size() - 2); i++)
	{
		// Bezier between p1 and p2
		glm::fvec4 p0 = points[i - 1]->getTransform().location;
		glm::fvec4 p1 = points[i]->getTransform().location;
		glm::fvec4 p2 = points[i + 1]->getTransform().location;
		glm::fvec4 p3 = points[i + 2]->getTransform().location;
		
		glm::fvec4 bl =	p0 / 3.0f        + p1 * 2.0f / 3.0f;
		glm::fvec4 br = p2 * 2.0f / 3.0f + p3 / 3.0f;

		glm::fvec4 b1 = p1 * 2.0f / 3.0f + p2 / 3.0f;
		glm::fvec4 b2 = p1 / 3.0f        + p2 * 2.0f / 3.0f;
		glm::fvec4 b0 = (bl + b1) / 2.0f;
		glm::fvec4 b3 = (br + b2) / 2.0f;
		beziers.emplace_back();
		beziers.rbegin()->UpdateMeshes({b0,b1,b2,b3});
	}
}

std::string CurveC2::getName() const
{
	return name;
}

void CurveC2::Render()
{
	chainMesh.Render();
	if (showChain)
		for (auto& b : beziers)
			b.chainMesh.Render();
	deBoorShader.use();

	unsigned int colorLoc = glGetUniformLocation(deBoorShader.ID, "color");
	glUniform4f(colorLoc, 0.0f, 1.0f, 1.0f, 1.0f);
	unsigned int transformLoc = glGetUniformLocation(deBoorShader.ID, "transform");
	glm::fmat4x4 centerMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(centerMatrix));
	for (auto& b : beziers)
		b.Render(deBoorShader);
}

void CurveC2::RenderGui()
{
	ImGui::Begin("C2 Curve");
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

void CurveC2::onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem)
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

void CurveC2::onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);

	auto it = std::find(points.begin(), points.end(), elem);
	if (it != points.end())
	{
		points.erase(it);
		UpdateMeshes();
	}
}

void CurveC2::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
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

void CurveC2::onMove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	if (std::find(points.begin(), points.end(), elem) != points.end())
		UpdateMeshes();
}
