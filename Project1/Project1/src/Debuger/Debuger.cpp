#include "Debuger.h"
#include "../Rendering/VariableManager.h"

std::vector<unsigned int> Debuger::textures;
std::vector<glm::fvec2> Debuger::uvPoints;
std::vector<glm::fvec3> Debuger::points;
std::vector<std::pair<glm::fvec3, glm::fvec3>> Debuger::vectors;
unsigned int Debuger::uvVAO,    Debuger::uvVBO,    Debuger::uvEBO, 
			 Debuger::pointVAO, Debuger::pointVBO, Debuger::pointEBO,
			 Debuger::vecVAO,   Debuger::vecVBO,   Debuger::vecEBO;
Shader Debuger::uvPointShader("Shaders/2dShader.vert","Shaders/fragmentShader.frag");
Shader Debuger::pointShader("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag");

void Debuger::updateUVPoints()
{
	std::vector<float> vert;
	std::vector<int> inds;
	vert.reserve(uvPoints.size() * 3);
	for (int i = 0; i<uvPoints.size(); i++)
	{
		vert.push_back(uvPoints[i].x);
		vert.push_back(uvPoints[i].y);
		vert.push_back(0);
		inds.push_back(i);
	}

	glBindVertexArray(uvVAO);
	glBindBuffer(GL_ARRAY_BUFFER, uvVBO);

	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uvEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Debuger::updatePoints()
{
	std::vector<float> vert;
	std::vector<int> inds;
	vert.reserve(points.size() * 3);
	for (int i = 0; i < points.size(); i++)
	{
		vert.push_back(points[i].x);
		vert.push_back(points[i].y);
		vert.push_back(points[i].z);
		inds.push_back(i);
	}

	glBindVertexArray(pointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);

	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pointEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Debuger::updateVectors()
{
	std::vector<float> vert;
	std::vector<int> inds;
	vert.reserve(points.size() * 3 * 2);
	for (int i = 0; i < vectors.size(); i++)
	{
		vert.push_back(vectors[i].first.x);
		vert.push_back(vectors[i].first.y);
		vert.push_back(vectors[i].first.z);
		vert.push_back(vectors[i].second.x);
		vert.push_back(vectors[i].second.y);
		vert.push_back(vectors[i].second.z);
		inds.push_back(2 * i);
		inds.push_back(2 * i + 1);
	}

	glBindVertexArray(vecVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vecVBO);

	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vecEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Debuger::Init()
{
	uvPointShader.Init();
	pointShader.Init();
	glGenVertexArrays(1, &uvVAO);
	glGenBuffers(1, &uvVBO);
	glGenBuffers(1, &uvEBO);
	glGenVertexArrays(1, &pointVAO);
	glGenBuffers(1, &pointVBO);
	glGenBuffers(1, &pointEBO);
	glGenVertexArrays(1, &vecVAO);
	glGenBuffers(1, &vecVBO);
	glGenBuffers(1, &vecEBO);
}

void Debuger::ShowTexture(unsigned int tex)
{
	textures.push_back(tex);
}

void Debuger::ShowUVPoint(glm::fvec2 point)
{
	uvPoints.push_back(point);
	updateUVPoints();
}

void Debuger::ShowUVSurface(std::shared_ptr<IUVSurface> s)
{
	const int sampleNumber = 50;
	for (int i = 0; i < sampleNumber; i++)
		for (int j = 0; j < sampleNumber; j++)
		{
			float u = i / (float)sampleNumber;
			float v = j / (float)sampleNumber;
			
			glm::fvec3 p = s->f(u, v);
			points.push_back(p);
			vectors.emplace_back(p, p + glm::normalize(s->dfdu(u,v)));
			vectors.emplace_back(p, p + glm::normalize(s->dfdv(u, v)));
		}
	updatePoints();
	updateVectors();
}

void Debuger::ShowPoint(glm::fvec3 point)
{
	points.push_back(point);
	updatePoints();
}

void Debuger::ShowVector(glm::fvec3 from, glm::fvec3 to)
{
	vectors.emplace_back(from,to);
	updateVectors();
}

bool Debuger::RenderGui()
{
	ImGui::Begin("Debuger");
	for (unsigned int tex : textures)
		ImGui::Image(ImTextureID(tex),ImVec2(500, 500));
	ImGui::End();
	return false;
}

void Debuger::Render(bool selected, VariableManager& vm)
{
	uvPointShader.use();
	vm.Apply(uvPointShader.ID);
	glBindVertexArray(uvVAO);
	glDrawElements(GL_POINTS, uvPoints.size(), GL_UNSIGNED_INT, 0);

	pointShader.use();
	vm.SetVariable("modelMtx", glm::identity<glm::fmat4x4>());
	vm.SetVariable("color", glm::fvec4(0.0f, 0.0f, 1.0f, 1.0f));
	vm.Apply(pointShader.ID);
	glBindVertexArray(pointVAO);
	glDrawElements(GL_POINTS, points.size(), GL_UNSIGNED_INT, 0);

	vm.SetVariable("color", glm::fvec4(1.0f, 0.0f, 0.0f, 1.0f));
	vm.Apply(pointShader.ID);
	glBindVertexArray(vecVAO);
	glDrawElements(GL_LINES, vectors.size()*2, GL_UNSIGNED_INT, 0);
}
