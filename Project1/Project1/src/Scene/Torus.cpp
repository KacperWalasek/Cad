#include "Torus.h"
#include "../Rendering/Sampler.h"
#include <imgui/imgui_stdlib.h>

void Torus::UpdateMesh()
{
	float eps = 0.1f;
	if (minorDencity < 3)
		minorDencity = 3;
	if (majorDencity < 3)
		majorDencity = 3;
	if (geometry.minorR > geometry.majorR)
		geometry.minorR = geometry.majorR - eps;
	if (geometry.majorR < eps)
		geometry.majorR = 2* eps;
	if (geometry.minorR < eps)
		geometry.minorR = eps;
	mesh = Sampler().sampleTorus(geometry, majorDencity, minorDencity);
	mesh.Update();

}

Torus::Torus()
	: geometry(20,10), majorDencity(10), minorDencity(10), name("Torus")
{
	UpdateMesh();
}

void Torus::RenderGui()
{
	ImGui::Begin("Torus"); 
	ImGui::InputText("Name", &name);
	transform.RenderGui();
	if (ImGui::InputInt("Minor dencity", &minorDencity))
		UpdateMesh();
	if (ImGui::InputInt("Major dencity", &majorDencity))
		UpdateMesh();
	if (ImGui::InputFloat("Minor radius", &(geometry.minorR)))
		UpdateMesh();
	if (ImGui::InputFloat("Major radius", &(geometry.majorR)))
		UpdateMesh();

	ImGui::End();
}

Transform& Torus::getTransform()
{
	return transform;
}

void Torus::Render()
{
	mesh.Render();
}
