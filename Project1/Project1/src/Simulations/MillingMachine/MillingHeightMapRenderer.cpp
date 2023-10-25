#include "MillingHeightMapRenderer.h"

void MillingHeightMapRenderer::flush()
{
	std::vector<float> vertices;
	vertices.reserve(vert.size() + temporaryVert.size());
	vertices.insert(vertices.end(), vert.begin(), vert.end());
	vertices.insert(vertices.end(), temporaryVert.begin(), temporaryVert.end());

	std::vector<int> indices;
	indices.reserve(inds.size() + temporaryInds.size());
	indices.insert(indices.end(), inds.begin(), inds.end());
	indices.insert(indices.end(), temporaryInds.begin(), temporaryInds.end());

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void MillingHeightMapRenderer::addSegment(std::vector<float>& vertices, std::vector<int>& indices, glm::fvec3 p1, glm::fvec3 p2, float r)
{
	clearTemporary();

	if (p1.x == p2.x && p1.y == p2.y)
		return;
	glm::fvec3 pVec = p2 - p1;
	glm::fvec3 perp1 = glm::normalize(glm::fvec3(-pVec.y, pVec.x, 0));
	glm::fvec3 perp2 = glm::normalize(glm::fvec3(pVec.y, -pVec.x, 0));

	glm::fvec3 p11 = p1 + perp1 * r;
	glm::fvec3 p12 = p1 + perp2 * r;
	glm::fvec3 p21 = p2 + perp1 * r;
	glm::fvec3 p22 = p2 + perp2 * r;

	int indOffset = vert.size() / 5;
	vertices.insert(vertices.end(), {
		p11.x, p11.y, (1.0f + p11.z) / 2.0f, 0, 0,
		p21.x, p21.y, (1.0f + p21.z) / 2.0f, 1, 0,
		p22.x, p22.y, (1.0f + p22.z) / 2.0f, 1, 1,
		p12.x, p12.y, (1.0f + p12.z) / 2.0f, 0, 1
		});
	indices.insert(indices.end(), {
		indOffset, indOffset + 1, indOffset + 2,
		indOffset, indOffset + 2, indOffset + 3
		});
	flush();
}

MillingHeightMapRenderer::MillingHeightMapRenderer()
	: shader("Shaders/MillingPath/millingPath.vert", "Shaders/MillingPath/millingPath.frag")
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

void MillingHeightMapRenderer::clearTemporary()
{
	temporaryVert.clear();
	temporaryInds.clear();
}

void MillingHeightMapRenderer::AddSegment(glm::fvec3 p1, glm::fvec3 p2, float r)
{
	addSegment(vert, inds, p1, p2, r);
}

void MillingHeightMapRenderer::AddTemporarySegment(glm::fvec3 p1, glm::fvec3 p2, float r)
{
	addSegment(temporaryVert, temporaryInds, p1, p2, r);
}

void MillingHeightMapRenderer::Render(bool selected, VariableManager& vm)
{
	shader.use();
	glBindVertexArray(VAO);
	vm.SetVariable("color", glm::fvec4(1, 0, 0, 1));
	vm.SetVariable("radius", radius);
	vm.SetVariable("flatMilling", flat);
	vm.Apply(shader.ID);
	glDrawElements(GL_TRIANGLES, inds.size() + temporaryInds.size(), GL_UNSIGNED_INT, 0);
}

void MillingHeightMapRenderer::setRadius(float radius)
{
	this->radius = radius;
}

void MillingHeightMapRenderer::setFlat(bool flat)
{
	this->flat = flat;
}
