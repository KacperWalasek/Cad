#include "Torus.h"
#include "../Rendering/Sampler.h"


void Torus::UpdateMesh()
{
	mesh = Sampler().sampleTorus(geometry, majorDencity, minorDencity);
	mesh.Init();

}

Torus::Torus()
	: geometry(20,10), majorDencity(100), minorDencity(100)
{
	UpdateMesh();
}

void Torus::RenderGui()
{
	ImGui::Begin("Torus");
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

Mesh& Torus::getMesh()
{
	return mesh;
}
