#include "InterpolationCurve.h"
#include <imgui/imgui_stdlib.h>
#include "../../Scene/Scene.h"
#include <glm/gtc/type_ptr.hpp>
#include "../../Math/CadMath.h"
Indexer InterpolationCurve::indexer;

InterpolationCurve::InterpolationCurve(std::vector<std::shared_ptr<Point>> points)
	:InterpolationCurve()
{
	this->points = points;
	this->name = "InterpolationCurve-" + std::to_string(indexer.getNewIndex());

	UpdateMeshes();
}

InterpolationCurve::InterpolationCurve(nlohmann::json json, std::map<int, std::shared_ptr<Point>>& pointMap)
	:InterpolationCurve()
{
	for (auto pi : json["controlPoints"])
		points.push_back(pointMap[pi["id"]]);
	if (json.contains("name"))
		name = json["name"];
	else
		name = "InterpolationCurve-" + std::to_string(indexer.getNewIndex());

	UpdateMeshes();
}
InterpolationCurve::InterpolationCurve() 
	: addSelected(false), removeSelected(false), showChain(false),
	shader("Shaders/test.vert", "Shaders/fragmentShader.frag")

{
	shader.Init();
	shader.loadShaderFile("Shaders/test.tesc", GL_TESS_CONTROL_SHADER);
	shader.loadShaderFile("Shaders/test.tese", GL_TESS_EVALUATION_SHADER);

}

std::string InterpolationCurve::getName() const
{
	return name;
}

void InterpolationCurve::Render(bool selected, VariableManager& vm)
{
	if (showChain)
		for (auto& b : beziers)
			b.chainMesh.Render();

	shader.use();
	for (auto& b : beziers)
		b.Render(shader, vm);
}

bool InterpolationCurve::RenderGui()
{
	ImGui::Begin("C0 Curve");
	ImGui::InputText("Name", &name);

	if (ImGui::Checkbox("Add on select", &addSelected))
		if (addSelected)
			removeSelected = false;
	if (ImGui::Checkbox("Remove on select", &removeSelected))
		if (removeSelected)
			addSelected = false;
	if (ImGui::Checkbox("Chorn knots", &chord))
		UpdateMeshes();

	ImGui::Checkbox("Show chain", &showChain);
	ImGui::End();
	return false;
}

void InterpolationCurve::UpdateMeshes()
{
	beziers.clear();


	std::vector<glm::fvec3> p;
	for (int i = 0; i < points.size(); i++)
	{
		if(i==0 || points[i-1]->getTransform().location != points[i]->getTransform().location)
			p.push_back(points[i]->getTransform().location);

	}
	if (p.size() < 2)
		return;
	int n = p.size() - 2;

	std::vector<float> d(n+1);
	d[0] = chord ? sqrtf(glm::dot(p[1] - p[0],p[1] - p[0])) : 1;

	std::vector<float> a1(n);
	std::vector<float> a2(n);
	std::vector<float> a3(n);
	std::vector<glm::fvec3> r(n);
	for (int i = 0; i < n; i++)
	{
		d[i + 1] = chord ? sqrtf(glm::dot(p[i + 2] - p[i + 1], p[i + 2] - p[i + 1])) : 1;
		
		a2[i] = 2;
		if (i != 0)
			a1[i - 1] = d[i] / (d[i] + d[i+1]);
		a3[i] = d[i + 1] / (d[i] + d[i + 1]);

		r[i] = 3.0f * (
			(p[i + 2] - p[i + 1]) / d[i + 1] -
			(p[i + 1] - p[i]) / d[i]
			) / (d[i] + d[i + 1]);
	}
	std::vector<glm::fvec3> fc;
	if(r.size()!=0)
		fc = CadMath::SolveTridiagonalMatrix(a1, a2, a3, r);
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
	
	for (int i = 0; i < n + 1; i++)
	{
		std::vector<glm::vec4> bezierPts = CadMath::PowerToBernstain({
			fa[i], 
			fb[i] * d[i], 
			fc[i] * d[i] * d[i], 
			fd[i] * d[i] * d[i] * d[i] 
			});
		beziers.emplace_back();
		beziers[i].UpdateMeshes(bezierPts);
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

nlohmann::json InterpolationCurve::Serialize(Scene& scene, Indexer& indexer, std::map<int, int>& pointIndexMap) const
{
	std::vector<nlohmann::json> pointIds;
	for (auto& p : points)
		pointIds.push_back({ { "id",pointIndexMap.find(p->getId())->second } });

	return {
		{"objectType", "interpolatedC2"},
		{"id", indexer.getNewIndex()},
		{"name", name},
		{"controlPoints", pointIds }
	};
}

void InterpolationCurve::onCollapse(Scene& scene, std::vector<std::shared_ptr<Point>>& collapsed, std::shared_ptr<Point> result)
{
	for (int i = 0; i < points.size(); i++)
		for (auto& c : collapsed)
			if (points[i] == c)
				points[i] = result;
	UpdateMeshes();
}

void InterpolationCurve::SelectAll(Scene& scene) const
{
	for (auto& p : points)
	{
		auto it = std::find_if(scene.objects.begin(), scene.objects.end(),
			[&p](const std::pair<std::shared_ptr<ISceneElement>, bool>& el) {
				return el.first.get() == p.get();
			});
		if (it != scene.objects.end())
			it->second = true;
	}
}

bool InterpolationCurve::CanChildBeDeleted() const
{
	return true;
}
