#include "Curve.h"
#include "../Rendering/Sampler.h"
#include <imgui/imgui_stdlib.h>
#include <cmath>

Curve::Curve()
{
}

Curve::Curve(std::vector<std::shared_ptr<Point>> points)
	:points(points)
{
	UpdateMeshes();
}

std::string Curve::getName() const
{
	return name;
}

Transform& Curve::getTransform()
{
	return transform;
}

void Curve::Render()
{
	UpdateMeshes();
	mesh.Render();
}

void Curve::RenderGui()
{
	ImGui::Begin("C0 Curve");
	ImGui::InputText("Name", &name);
	ImGui::End();
}

void Curve::UpdateMeshes()
{
	beziers.clear();
	mesh = Mesh();
	for (int i = 0; i < points.size() / 3.0f; i++)
	{
		std::vector<glm::fvec4> bp;
		for (int j = 0; j < glm::min(4, (int)points.size() - i * 3); j++)
			bp.push_back(points[i * 3 + j]->getTransform().location);
		beziers.push_back({ bp });
	}
	for (BezierCurve& bezier : beziers) 
		mesh = Mesh::CombineMeshes(mesh, Sampler().sampleBezierCurve(bezier, 20));
	mesh.Update();
}

void Curve::onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);
	if(p)
		points.push_back(p);
}

void Curve::onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);
}

void Curve::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);
	if(p)
		points.push_back(p);
}
