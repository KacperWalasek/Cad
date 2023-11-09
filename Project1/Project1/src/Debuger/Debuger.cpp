#include "Debuger.h"
#include "../Rendering/VariableManager.h"

std::vector<unsigned int> Debuger::textures;
std::vector<glm::fvec2> Debuger::uvPoints;
unsigned int Debuger::VAO, Debuger::VBO, Debuger::EBO;
Shader Debuger::uvPointShader("Shaders/2dShader.vert","Shaders/fragmentShader.frag");

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

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Debuger::Init()
{
	uvPointShader.Init();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
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
	glBindVertexArray(VAO);
	glDrawElements(GL_POINTS, uvPoints.size(), GL_UNSIGNED_INT, 0);
}
