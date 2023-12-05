#include "SurfaceShift.h"

glm::fvec3 SurfaceShift::normal(float u, float v) const
{
	auto du = surface->dfdu(u, v);
	auto dv = surface->dfdv(u, v);
	if (glm::length(du) == 0 || glm::length(dv) == 0)
		return { 0,0,-1 };
	return glm::normalize(glm::cross(surface->dfdu(u, v), surface->dfdv(u, v)));
}

void SurfaceShift::updateMesh(bool reverse)
{
	std::vector<float> vertices;
	std::vector<int> indices;

	for (int i = 0; i < renderDetails; i++)
	{
		for (int j = 0; j < renderDetails; j++)
		{
			float startU = i / (float)(renderDetails);
			float startV = j / (float)(renderDetails);
			float endU = (i + 1) / (float)(renderDetails);
			float endV = (j + 1) / (float)(renderDetails);

			glm::fvec3 p00 = f(startU, startV);
			glm::fvec3 p10 = f(endU, startV);
			glm::fvec3 p01 = f(startU, endV);
			glm::fvec3 p11 = f(endU, endV);

			int indexOffset = vertices.size()/3;
			vertices.insert(vertices.end(), {
				p00.x,p00.y,p00.z,
				p10.x,p10.y,p10.z,
				p11.x,p11.y,p11.z,
				p01.x,p01.y,p01.z
				});

			if (reverse)
				indices.insert(indices.end(), {
					indexOffset, indexOffset + 1, indexOffset + 2,
					indexOffset, indexOffset + 2, indexOffset + 3
					});
			else
				indices.insert(indices.end(), {
					indexOffset, indexOffset + 2, indexOffset + 1,
					indexOffset, indexOffset + 3, indexOffset + 2
					});
		}
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	indicesSize = indices.size();
}

SurfaceShift::SurfaceShift(std::shared_ptr<IUVSurface> surface, float shift, bool reverse)
	: surface(surface), shift(shift), reverse(reverse), 
	shader("Shaders/position.vert","Shaders/height.frag")
{
	shader.Init();
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	updateMesh(reverse);
}

glm::fvec3 SurfaceShift::f(float u, float v) const
{
	glm::fvec3 n = normal(u, v);
	if (reverse)
		n = -n;
	return surface->f(u,v) + n * shift;
}

glm::fvec3 SurfaceShift::dfdu(float u, float v) const
{
	glm::fvec3 sdfdu = surface->dfdu(u, v);
	glm::fvec3 sdfdv = surface->dfdv(u, v);
	glm::fvec3 sdfduu = surface->dfduu(u, v);
	glm::fvec3 sdfduv = surface->dfduv(u, v);
	glm::fvec3 sdfdvv = surface->dfdvv(u, v);

	glm::fvec3 norm = glm::cross(sdfdu, sdfdv);
	float normnorm = glm::length(norm);
	glm::fvec3 normU = glm::cross(sdfduu, sdfdv) + glm::cross(sdfdu, sdfduv);
	return sdfdu;//  +(normU / normnorm - norm * glm::dot(norm, normU) / (normnorm * normnorm * normnorm)) * shift;
}

glm::fvec3 SurfaceShift::dfdv(float u, float v) const
{
	glm::fvec3 sdfdu = surface->dfdu(u, v);
	glm::fvec3 sdfdv = surface->dfdv(u, v);
	glm::fvec3 sdfduu = surface->dfduu(u, v);
	glm::fvec3 sdfduv = surface->dfduv(u, v);
	glm::fvec3 sdfdvv = surface->dfdvv(u, v);

	glm::fvec3 norm = glm::cross(sdfdu, sdfdv);
	float normnorm = glm::length(norm);
	glm::fvec3 normV = glm::cross(sdfduv, sdfdv) + glm::cross(sdfdu, sdfdvv);
	return sdfdv + (normV / normnorm - norm * glm::dot(norm, normV) / (normnorm * normnorm * normnorm))*shift;
}

bool SurfaceShift::wrappedU()
{
	return surface->wrappedU();
}

bool SurfaceShift::wrappedV()
{
	return surface->wrappedV();
}

void SurfaceShift::acceptIntersection(std::weak_ptr<Intersection> intersection)
{
	return surface->acceptIntersection(intersection);
}

void SurfaceShift::removeIntersection(std::weak_ptr<Intersection> intersection)
{
	return surface->removeIntersection(intersection);
}

const std::vector<std::weak_ptr<Intersection>>& SurfaceShift::getIntersections()
{
	return surface->getIntersections();
}

void SurfaceShift::setRenderState(SurfaceRenderState state)
{
	return surface->setRenderState(state);
}

glm::fvec3 SurfaceShift::dfduu(float u, float v) const
{
	return surface->dfduu(u, v);
}

glm::fvec3 SurfaceShift::dfduv(float u, float v) const
{
	return surface->dfduv(u, v);
}

glm::fvec3 SurfaceShift::dfdvv(float u, float v) const
{
	return surface->dfdvv(u, v);
}

void SurfaceShift::Render(bool selected, VariableManager& vm)
{
	shader.use();
	vm.SetVariable("modelMtx", glm::identity<glm::fmat4x4>());
	vm.Apply(shader.ID);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
}
