#include "Curve.h"
#include "../Rendering/Sampler.h"
#include <imgui/imgui_stdlib.h>
#include <cmath>
#include "Scene.h"
#include <algorithm>
#include <cmath>

int Curve::calculateDivision(BezierCurve& bezier)
{
	std::vector<glm::fvec4> projBeziers;
	std::transform(bezier.points.begin(),bezier.points.end(), std::back_insert_iterator(projBeziers),
		[this](const glm::fvec4& p) {
			glm::fvec4 projP = camera.GetProjectionMatrix() * camera.GetViewMatrix() * glm::fvec4(p.x, p.y, p.z, 1);
			return projP/projP.w; 
		});
	float len = 0;
	for (int i = 0; i < projBeziers.size() - 1; i++)
		len += sqrt(powf(projBeziers[i].x, 2) + powf(projBeziers[i].y, 2));
	
	return fmin(maxDivision,fmax(minDivision,ceil(len * divisionMultiplyer)));
}
Curve::Curve(Camera& camera)
	:name("Curve"), addSelected(false), removeSelected(false), showChain(false), camera(camera)
{
}

Curve::Curve(Camera& camera, std::vector<std::shared_ptr<Point>> points)
	: camera(camera),points(points), name("Curve"), addSelected(false), removeSelected(false), showChain(false)
{
	UpdateMeshes();
}

std::string Curve::getName() const
{
	return name;
}

void Curve::Render()
{
	UpdateMeshes();
	mesh.Render();
	if (showChain)
		chainMesh.Render();
}

void Curve::RenderGui()
{
	ImGui::Begin("C0 Curve");
	ImGui::InputText("Name", &name);

	if (ImGui::Checkbox("Add on select", &addSelected))
		if (addSelected)
			removeSelected = false;
	if(ImGui::Checkbox("Remove on select", &removeSelected))
		if (removeSelected)
			addSelected = false;

	ImGui::Checkbox("Show chain", &showChain);
	ImGui::End();
}

void Curve::UpdateMeshes()
{
	beziers.clear();
	mesh = Mesh();
	chainMesh = Mesh();
	for (int i = 0; i < points.size() / 3.0f; i++)
	{
		std::vector<glm::fvec4> bp;
		for (int j = 0; j < glm::min(4, (int)points.size() - i * 3); j++)
			bp.push_back(points[i * 3 + j]->getTransform().location);
		if (bp.size() > 1)
			beziers.push_back({ bp });
	}

	for (BezierCurve& bezier : beziers) 
	{
		mesh = Mesh::CombineMeshes(mesh, Sampler().sampleBezierCurve(bezier, calculateDivision(bezier)));
		Mesh subchain;
		for (int i = 0; i<bezier.points.size(); i++)
		{
			subchain.vertices.push_back(bezier.points[i].x);
			subchain.vertices.push_back(bezier.points[i].y);
			subchain.vertices.push_back(bezier.points[i].z);

			if(i<bezier.points.size()-1)
			{
				subchain.indices.push_back(i);
				subchain.indices.push_back(i + 1);
			}
		}
		chainMesh = Mesh::CombineMeshes(chainMesh, subchain);
	}
	mesh.Update();
	chainMesh.Update();


}


void Curve::onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	auto curveAsSceneElem = std::find_if(scene.objects.begin(), scene.objects.end(),
		[this](const std::pair<std::shared_ptr<ISceneElement>, bool>& obj)
		{
			return obj.first.get() == this;
		});
	if(curveAsSceneElem!=scene.objects.end() && curveAsSceneElem->second)
	{
		std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);
		if (p)
			points.push_back(p);
	}
}

void Curve::onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);
	
	auto it = std::find(points.begin(), points.end(), elem);
	if (it != points.end())
		points.erase(it);
		
}

void Curve::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
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
					points.push_back(p);
			}
		}
		else
			if (removeSelected)
			{
				points.erase(pointIt);
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
