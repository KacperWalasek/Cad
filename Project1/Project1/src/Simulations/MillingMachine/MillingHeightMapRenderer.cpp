#include "MillingHeightMapRenderer.h"

void MillingHeightMapRenderer::flush()
{
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

MillingHeightMapRenderer::MillingHeightMapRenderer()
	: shader("Shaders/test.vert", "Shaders/baseColor.frag")
{
	shader.Init();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
}

void MillingHeightMapRenderer::Clear()
{
	vert.clear();
	inds.clear();
	flush();
}

void MillingHeightMapRenderer::AddSegment(glm::fvec3 p1, glm::fvec3 p2, float r)
{
	if (p1.x == p2.x && p1.y == p2.y)
		return;
	glm::fvec3 pVec = p2 - p1;
	glm::fvec3 perp1 = glm::normalize(glm::fvec3(-pVec.y, pVec.x,0));
	glm::fvec3 perp2 = glm::normalize(glm::fvec3(pVec.y, -pVec.x,0));

	glm::fvec3 p11 = p1 + perp1*r;
	glm::fvec3 p12 = p1 + perp2*r;
	glm::fvec3 p21 = p2 + perp1*r;
	glm::fvec3 p22 = p2 + perp2*r;

	int indOffset = vert.size()/3;
	vert.insert(vert.end(), {
		p11.x, p11.y, (1.0f+p11.z) / 2.0f,
		p21.x, p21.y, (1.0f+p21.z) / 2.0f,
		p22.x, p22.y, (1.0f+p22.z) / 2.0f,
		p12.x, p12.y, (1.0f+p12.z) / 2.0f,
		});
	inds.insert(inds.end(), {
		indOffset, indOffset+1, indOffset+2,
		indOffset, indOffset+2, indOffset+3
		});
	flush();
}

void MillingHeightMapRenderer::Render(bool selected, VariableManager& vm)
{
	shader.use();
	glBindVertexArray(VAO);
	vm.SetVariable("color", glm::fvec4(1, 0, 0, 1));
	vm.Apply(shader.ID);
	glDrawElements(GL_TRIANGLES, inds.size(), GL_UNSIGNED_INT, 0);
}
