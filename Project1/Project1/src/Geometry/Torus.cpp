#include "Torus.h"
#include "../Rendering/Sampler.h"
#include <imgui/imgui_stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

Indexer Torus::indexer;
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

	std::vector<float> vert;
	std::vector<int> inds;

	int countI = majorDencity + 1;
	int countJ = minorDencity + 1;
	for (int i = 0; i < countI; i++)
	{
		for (int j = 0; j < countJ; j++)
		{
			float u = i / (float)majorDencity;
			float v = j / (float)minorDencity;
			glm::fvec3 p = geometry.getPointOnMesh(u * 2 * M_PI, v * 2 * M_PI);

			vert.insert(vert.end(), {
				p.x,p.y,p.z, 
				u,v
			});

			if (j != minorDencity)
				inds.insert(inds.end(), {
					i * countJ + j + 1,
					i * countJ + j,
					});

			if (i != majorDencity)
				inds.insert(inds.end(), {
					i * countJ + j,
					(i + 1) * countJ + j
				});
		}
	}
	elementSize = inds.size();
	
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);


}

Torus::Torus()
	: geometry(20,10), majorDencity(10), minorDencity(10), name("Torus-" + std::to_string(indexer.getNewIndex())),
	shader("Shaders/torus.vert", "Shaders/uv.frag")
{
	shader.Init();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	UpdateMesh();
}

Torus::Torus(nlohmann::json json)
	:geometry(json["largeRadius"], json["smallRadius"]), 
	majorDencity(json["samples"]["x"]), minorDencity(json["samples"]["y"]),
	transform(json),
	shader("Shaders/torus.vert", "Shaders/uv.frag")
{
	shader.Init();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	if (json.contains("name"))
		name = json["name"];
	else
		name = "Torus-" + std::to_string(indexer.getNewIndex());
	UpdateMesh();
}

bool Torus::RenderGui()
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
	return false;
}

Transform& Torus::getTransform()
{
	return transform;
}

void Torus::Render(bool selected, VariableManager& vm)
{
	shader.use();
	if(intersectionTextures.size() !=0)
	{
		auto intersection = intersections[0].lock();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, intersectionTextures[0]);

		vm.SetVariable("reverseIntersect", intersection->reverse);
		vm.SetVariable("intersect", intersection->intersect);
	}
	vm.Apply(shader.ID);
	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, elementSize, GL_UNSIGNED_INT, 0);
}

nlohmann::json Torus::Serialize(Scene& scene, Indexer& indexer, std::map<int, int>& pointIndexMap) const
{
	nlohmann::json transformJson = transform.Serialize();
	nlohmann::json t = {
		{"objectType", "torus"},
		{"id", indexer.getNewIndex()},
		{"name", name},
		{"position", transformJson["position"]},
		{"rotation", transformJson["rotation"]},
		{"scale", transformJson["scale"]},
		{"samples",
			{
				{"x", minorDencity},
				{"y", majorDencity}
			}
		},
		{"smallRadius", geometry.minorR},
		{"largeRadius", geometry.majorR}
	};
	return t;
}

glm::fvec3 Torus::f(float u, float v) const
{
	glm::fvec4 p = transform.GetMatrix() *  geometry.getPointOnMesh(u * 2 * M_PI, v * 2 * M_PI);
	return p.xyz();
}

glm::fvec3 Torus::dfdu(float u, float v) const
{
	float alpha = 2 * M_PI * u;
	float beta = 2 * M_PI * v;

	glm::fvec3 untransformed = 2.0f * (float)M_PI * (geometry.majorR + cosf(beta) * geometry.minorR) * glm::fvec3(
		-sinf(alpha),
		0,
		cosf(alpha)
	);
	return (transform.GetMatrix() * glm::fvec4(untransformed, 0.0f)).xyz();
}

glm::fvec3 Torus::dfdv(float u, float v) const
{
	float alpha = 2 * M_PI * u;
	float beta = 2 * M_PI * v;

	glm::fvec3 untransformed = 2.0f * (float)M_PI * geometry.minorR * glm::fvec3(
		-cosf(alpha) * sinf(beta),
		 cosf(beta),
		-sinf(alpha) * sinf(beta)
	);
	return (transform.GetMatrix() * glm::fvec4(untransformed, 0.0f)).xyz();
}

bool Torus::wrappedU()
{
	return true;
}

bool Torus::wrappedV()
{
	return true;
}

void Torus::acceptIntersection(std::weak_ptr<Intersection> intersection)
{
	auto intLock = intersection.lock();

	intersections.push_back(intersection);

	if (this == intLock->s1.get())
		intersectionTextures.push_back(intLock->uvS1Tex);

	if (this == intLock->s2.get())
		intersectionTextures.push_back(intLock->uvS2Tex);
}

void Torus::removeIntersection(std::weak_ptr<Intersection> intersection)
{
	auto intLock = intersection.lock();

	std::erase_if(intersections, [intLock](const std::weak_ptr<Intersection>& i) { 
		return i.lock().get() == intLock.get(); 
	});
	std::erase_if(intersectionTextures, [&intLock](const unsigned int& i) { 
		return i == intLock->uvS1Tex || i == intLock->uvS2Tex; 
	});
}
