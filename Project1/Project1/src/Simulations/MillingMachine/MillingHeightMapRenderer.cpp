#include "MillingHeightMapRenderer.h"

float MillingHeightMapRenderer::stretchZ(float z) const
{
	return z;
	float rangeMin = normalizeZ(path.zRange.x);
	float rangeMax = normalizeZ(path.zRange.y);
	return (z - rangeMin) / (rangeMax - rangeMin);
}

float MillingHeightMapRenderer::normalizeZ(float z) const
{
	return (1.0f + z) / 2.0f;
}

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
	//  x,     y,     z,                           u, v
		p11.x, p11.y, stretchZ(normalizeZ(p11.z)), 0, 0,
		p21.x, p21.y, stretchZ(normalizeZ(p21.z)), 1, 0,
		p22.x, p22.y, stretchZ(normalizeZ(p22.z)), 1, 1,
		p12.x, p12.y, stretchZ(normalizeZ(p12.z)), 0, 1
		});
	indices.insert(indices.end(), {
		indOffset, indOffset + 1, indOffset + 2,
		indOffset, indOffset + 2, indOffset + 3
		});
}

MillingHeightMapRenderer::MillingHeightMapRenderer(MillingPath path)
	: shader("Shaders/MillingPath/millingPath.vert", "Shaders/MillingPath/millingPath.frag"),
	path(path)
{
	shader.Init();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
}

glm::fvec3 MillingHeightMapRenderer::SetDistance(float distance)
{
	auto [prevIndex, nextIndex, currentPosition] = getPosition(distance);

	if (lastVisited != prevIndex)
	{
		for (int i = lastVisited; i < prevIndex; i++)
			addFixedSegment(path.positions[i] * sizeMultiplier,
				path.positions[i + 1] * sizeMultiplier,
				path.radius * sizeMultiplier);
		lastVisited = prevIndex;
	}

	if (nextIndex == path.positions.size() - 1 && path.positions[prevIndex] == currentPosition)
		lastVisited = path.positions.size() - 1;
	else
		addTemporarySegment(path.positions[prevIndex] * sizeMultiplier,
			currentPosition * sizeMultiplier,
			path.radius * sizeMultiplier);

	flush();
	return currentPosition;
}

glm::fvec3 MillingHeightMapRenderer::Finalize()
{
	for (int i = lastVisited; i < path.positions.size() - 1; i++)
		addFixedSegment(path.positions[i] * sizeMultiplier,
			path.positions[i + 1] * sizeMultiplier,
			path.radius * sizeMultiplier);
	lastVisited = path.positions.size() - 1;
	flush();
	return *path.positions.rbegin();
}

float MillingHeightMapRenderer::GetLength()
{
	return path.totalLength;
}

PathState MillingHeightMapRenderer::GetState() const
{
	if (vert.empty() && temporaryVert.empty())
		return PathState::Off;
	if (lastVisited == path.positions.size() - 1)
		return PathState::Finished;
	return PathState::Running;
}

bool MillingHeightMapRenderer::IsFlat() const
{
	return path.flat;
}

float MillingHeightMapRenderer::GetRadius() const
{
	return path.radius;
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

std::tuple<int, int, glm::fvec3> MillingHeightMapRenderer::getPosition(float distance)
{
	float distNorm = distance / (path.totalLength * sizeMultiplier);

	for (int i = 0; i < path.dists.size() - 1; i++)
	{
		if (path.dists[i] > distNorm)
		{
			float distDiff = path.dists[i] - path.dists[i - 1];
			float wsp = (distNorm - path.dists[i - 1]) / distDiff;
			return { i - 1,i, wsp * path.positions[i] + (1 - wsp) * path.positions[i - 1] };
		}
	}
	return { path.positions.size() - 2, path.positions.size() - 1, path.positions[path.positions.size() - 1] };

}

void MillingHeightMapRenderer::addFixedSegment(glm::fvec3 p1, glm::fvec3 p2, float r)
{
	addSegment(vert, inds, p1, p2, r);
}

void MillingHeightMapRenderer::addTemporarySegment(glm::fvec3 p1, glm::fvec3 p2, float r)
{
	addSegment(temporaryVert, temporaryInds, p1, p2, r);
}

void MillingHeightMapRenderer::Render(bool selected, VariableManager& vm)
{
	shader.use();
	glBindVertexArray(VAO);
	vm.SetVariable("color", glm::fvec4(1, 0, 0, 1));
	vm.SetVariable("radius", path.radius * sizeMultiplier);
	vm.SetVariable("flatMilling", path.flat);
	vm.Apply(shader.ID);
	glDrawElements(GL_TRIANGLES, inds.size() + temporaryInds.size(), GL_UNSIGNED_INT, 0);
}