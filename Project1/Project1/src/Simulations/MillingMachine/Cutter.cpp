#include "Cutter.h"
#include <GL/glew.h>
#include <vector>
#include <numbers>

void Cutter::createMesh(int divisions)
{
	indexCount = (divisions+1) * 4 * 3;
	int vertCount = (divisions + 2) * 2;
	std::vector<float> vert;
	std::vector<int> inds;
	vert.reserve(vertCount * 6);
	inds.reserve(indexCount);
	vert.insert(vert.end(), {
		0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		});
	for (int i = 0; i < divisions+1; i++)
	{
		auto [baseX,baseZ] = sampleBase(divisions, i);
		vert.insert(vert.end(), {
			baseX, 0.0f,  baseZ, baseX, 0.0f, baseZ,
			baseX, 10.0f, baseZ, baseX, 0.0f, baseZ,
			});

		inds.insert(inds.end(), {
			2 + i * 2, 4 + i * 2, 0,
			1, 3 + i * 2, 5 + i * 2,
			3 + i * 2, 2 + i * 2, 4 + i * 2,
			5 + i * 2, 3 + i * 2, 4 + i * 2,
			});
	}
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

std::tuple<float, float> Cutter::sampleBase(int divisions, int index)
{
	float angle = 2.0f * std::numbers::pi * index / (float)divisions;
	return { cosf(angle), sinf(angle) };
}

Cutter::Cutter()
	: shader("Shaders/Phong/phong.vert","Shaders/Phong/phong.frag")
{
	shader.Init();
	createMesh(20);
}

void Cutter::setPosition(glm::fvec3 pos)
{
	translation = glm::translate(glm::scale(glm::identity<glm::fmat4x4>(), glm::fvec3(0.05, 0.05, 0.05)), pos*20.0f);
}

void Cutter::Render(bool selected, VariableManager& vm)
{
	shader.use();
	glBindVertexArray(VAO);
	vm.SetVariable("color", glm::fvec4(0.5, 0.5, 0.8, 0));
	vm.SetVariable("modelMtx", translation);
	vm.Apply(shader.ID);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}
