#include "InterpolationCurve.h"
#include <imgui/imgui_stdlib.h>
#include "Scene.h"
#include <glm/gtc/type_ptr.hpp>
#include "../Math/CadMath.h"
Indexer InterpolationCurve::indexer;

InterpolationCurve::InterpolationCurve(Camera& camera)
	: InterpolationCurve(camera, {})
{
}

InterpolationCurve::InterpolationCurve(Camera& camera, std::vector<std::shared_ptr<Point>> points)
	: camera(camera), points(points), name("InterpolationCurve-" + std::to_string(indexer.getNewIndex())), addSelected(false),
	removeSelected(false), showChain(false),
	shader("Shaders/test.vert", "Shaders/fragmentShader.frag")
{
	shader.Init();
	shader.loadShaderFile("Shaders/test.tesc", GL_TESS_CONTROL_SHADER);
	shader.loadShaderFile("Shaders/test.tese", GL_TESS_EVALUATION_SHADER);

	UpdateMeshes();
}

std::string InterpolationCurve::getName() const
{
	return name;
}

void InterpolationCurve::Render(bool selected)
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

void InterpolationCurve::RenderGui()
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

void InterpolationCurve::UpdateMeshes()
{
	if (points.size() < 3)
		return;

	int n = points.size()-2;
	std::vector<glm::fvec3> p(points.size());
	for (int i = 0; i < points.size(); i++)
		p[i] = points[i]->getTransform().location;

	std::vector<glm::fvec3> d(n+1);
	//d[0] = (p[1] - p[0]) * (p[1] - p[0]);
	d[0] = { 1,1,1 };
	std::vector<glm::fvec3> a1(n);
	std::vector<glm::fvec3> a2(n);
	std::vector<glm::fvec3> a3(n);
	std::vector<glm::fvec3> r(n);
	for (int i = 0; i < n; i++)
	{
		//d[i+1] = (p[i+2] - p[i+1]) * (p[i+2] - p[i+1]);
		d[i+1] = { 1,1,1 };
		a2[i] = { 2,2,2 };
		if (i != 0)
			a1[i - 1] = d[i] / (d[i] + d[i+1]);
		a3[i] = d[i + 1] / (d[i] + d[i + 1]);

		r[i] = 3.0f * (
			(p[i + 2] - p[i + 1]) / d[i + 1] -
			(p[i + 1] - p[i]) / d[i]
			) / (d[i] + d[i + 1]);
	}
	auto fc = CadMath::SolveTridiagonalMatrix(a1, a2, a3, r);
	fc.push_back({ 0,0,0 });
	fc.insert(fc.begin(), {0,0,0});
	std::vector<glm::fvec3> fa = p;
	std::vector<glm::fvec3> fb(n+2);
	std::vector<glm::fvec3> fd(n+2);

	for (int i = 0; i < n + 1; i++)
	{
		fd[i] = (2.0f * fc[i + 1] - 2.0f * fc[i])/ (6.0f*d[i]);
		fb[i] = (fa[i + 1] - fa[i] - fc[i] * d[i] * d[i] - fd[i] * d[i] * d[i] * d[i]) / d[i];
	}
	
	std::vector<std::vector<glm::vec4>> bezierPoints(n+1);
	for (int i = 0; i < n + 1; i++)
	{
		bezierPoints[i] = CadMath::PowerToBernstain({ fa[i], fb[i], fc[i], fd[i] });
		beziers.emplace_back();
		beziers[i].UpdateMeshes(bezierPoints[i]);
	}
}


void InterpolationCurve::onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem)
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

void InterpolationCurve::onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);

	auto it = std::find(points.begin(), points.end(), elem);
	if (it != points.end())
	{
		points.erase(it);
		UpdateMeshes();
	}

}

void InterpolationCurve::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
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

void InterpolationCurve::onMove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	if (std::find(points.begin(), points.end(), elem) != points.end())
		UpdateMeshes();
}
