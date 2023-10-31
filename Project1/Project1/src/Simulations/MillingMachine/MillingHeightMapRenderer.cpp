#include "MillingHeightMapRenderer.h"
#include <numbers>

float MillingHeightMapRenderer::stretchZ(float z) const
{
	float rangeMin = 0;
	float rangeMax = materialSize.y*10.0f;
	return (z - rangeMin) / (rangeMax - rangeMin);
}

void MillingHeightMapRenderer::flush()
{
	// flush rectangles
	std::vector<float> vertices;
	vertices.reserve(vert_rect.size() + temporaryVert_rect.size());
	vertices.insert(vertices.end(), vert_rect.begin(), vert_rect.end());
	vertices.insert(vertices.end(), temporaryVert_rect.begin(), temporaryVert_rect.end());

	std::vector<int> indices;
	indices.reserve(inds_rect.size() + temporaryInds_rect.size());
	indices.insert(indices.end(), inds_rect.begin(), inds_rect.end());
	indices.insert(indices.end(), temporaryInds_rect.begin(), temporaryInds_rect.end());
	
	glBindVertexArray(VAO_rect);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_rect);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_rect);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// flush circles
	vertices.clear();
	vertices.reserve(vert_circ.size() + temporaryVert_circ.size());
	vertices.insert(vertices.end(), vert_circ.begin(), vert_circ.end());
	vertices.insert(vertices.end(), temporaryVert_circ.begin(), temporaryVert_circ.end());

	indices.clear();
	indices.reserve(inds_circ.size() + temporaryInds_circ.size());
	indices.insert(indices.end(), inds_circ.begin(), inds_circ.end());
	indices.insert(indices.end(), temporaryInds_circ.begin(), temporaryInds_circ.end());
	glBindVertexArray(VAO_circ);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_circ);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_circ);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void MillingHeightMapRenderer::addSegment(std::vector<float>& v_rect, std::vector<int>& i_rect, 
										  std::vector<float>& v_circ, std::vector<int>& i_circ,
										  glm::fvec3 p1, glm::fvec3 p2, float r)
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

	int indOffset = vert_rect.size() / 5;
	v_rect.insert(v_rect.end(), {
	//  x,     y,     z,              u, v
		p11.x, p11.y, stretchZ(p1.z), 0, 0,
		p21.x, p21.y, stretchZ(p2.z), 1, 0,
		p22.x, p22.y, stretchZ(p2.z), 1, 1,
		p12.x, p12.y, stretchZ(p1.z), 0, 1
		});
	i_rect.insert(i_rect.end(), {
		indOffset, indOffset + 1, indOffset + 2,
		indOffset, indOffset + 2, indOffset + 3
		});
	
	// circle
	indOffset = vert_circ.size() / 4;
	glm::fvec3 center = { p2.x,p2.y,stretchZ(p2.z) };
	v_circ.insert(v_circ.end(), {
	//	x,	  y,	z,	  dist
		center.x, center.y, center.z, 0.0f
		});
	for (int i = 0; i < circleDivisions + 1; i++) {
		float angle = i * 2.0f * std::numbers::pi / (float)circleDivisions;
		glm::fvec3 p = center + glm::fvec3(r * cosf(angle), r * sinf(angle), 0);
		v_circ.insert(v_circ.end(), {
		//	x,	 y,   z,   dist
			p.x, p.y, p.z, 1.0f
			});
		if (i != circleDivisions)
			i_circ.insert(i_circ.end(), {
				indOffset, indOffset + i + 2, indOffset + i + 1
				});

	}

}

MillingHeightMapRenderer::MillingHeightMapRenderer(MillingPath path, glm::fvec3 materialSize)
	: rectShader("Shaders/MillingPath/millingPath.vert", "Shaders/MillingPath/millingPath.frag"),
	circShader("Shaders/MillingPath/millingPathCirc.vert", "Shaders/MillingPath/millingPathCirc.frag"),
	path(path)
{
	rectShader.Init();
	circShader.Init();

	glGenVertexArrays(1, &VAO_rect);
	glGenBuffers(1, &VBO_rect);
	glGenBuffers(1, &EBO_rect);

	glGenVertexArrays(1, &VAO_circ);
	glGenBuffers(1, &VBO_circ);
	glGenBuffers(1, &EBO_circ);

	SetMaterialSize(materialSize);
}

glm::fvec3 MillingHeightMapRenderer::SetDistance(float distance)
{
	auto [prevIndex, nextIndex, currentPosition] = getPosition(distance);

	if (lastVisited != prevIndex)
	{
		for (int i = lastVisited; i < prevIndex; i++)
			addFixedSegment(path.positions[i] * sizeMultiplier,
				path.positions[i + 1] * sizeMultiplier,
				path.radius * sizeMultiplier.x);
		lastVisited = prevIndex;
	}

	if (nextIndex == path.positions.size() - 1 && path.positions[prevIndex] == currentPosition)
		lastVisited = path.positions.size() - 1;
	else
		addTemporarySegment(path.positions[prevIndex] * sizeMultiplier,
			currentPosition * sizeMultiplier,
			path.radius * sizeMultiplier.x);

	flush();
	return currentPosition;
}

void MillingHeightMapRenderer::SetMaterialSize(glm::fvec3 materialSize)
{
	this->materialSize = materialSize;
	sizeMultiplier = { 2.0f / (materialSize.x * 10.0f), 2.0f / (materialSize.x * 10.0f), 1.0f }; //TODO
}

glm::fvec3 MillingHeightMapRenderer::Finalize()
{
	for (int i = lastVisited; i < path.positions.size() - 1; i++)
		addFixedSegment(path.positions[i] * sizeMultiplier,
			path.positions[i + 1] * sizeMultiplier,
			path.radius * sizeMultiplier.x);
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
	if (vert_rect.empty() && temporaryVert_rect.empty())
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

const MillingPath& MillingHeightMapRenderer::GetPath() const
{
	return path;
}

void MillingHeightMapRenderer::Clear()
{
	vert_rect.clear();
	inds_rect.clear();
	vert_circ.clear();
	inds_circ.clear();
	flush();
}

void MillingHeightMapRenderer::clearTemporary()
{
	temporaryVert_rect.clear();
	temporaryInds_rect.clear();
	temporaryVert_circ.clear();
	temporaryInds_circ.clear();
}

std::tuple<int, int, glm::fvec3> MillingHeightMapRenderer::getPosition(float distance)
{
	float distNorm = distance / (path.totalLength * sizeMultiplier.x);

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
	addSegment(vert_rect, inds_rect, vert_circ, inds_circ, p1, p2, r);
} 

void MillingHeightMapRenderer::addTemporarySegment(glm::fvec3 p1, glm::fvec3 p2, float r)
{
	addSegment(temporaryVert_rect, temporaryInds_rect, temporaryVert_circ, temporaryInds_circ, p1, p2, r);
} 
  
void MillingHeightMapRenderer::renderAll(VariableManager& vm)
{   
	if (useExternalShader)
	{
		externalShaderRect->use();
		vm.Apply(externalShaderRect->ID);
	}
	else
	{
		rectShader.use();
		vm.Apply(rectShader.ID);
	}
	glBindVertexArray(VAO_rect);
	glDrawElements(GL_TRIANGLES, inds_rect.size() + temporaryInds_rect.size(), GL_UNSIGNED_INT, 0);

	if (useExternalShader)
	{
		externalShaderCirc->use();
		vm.Apply(externalShaderCirc->ID);
	}
	else
	{
		circShader.use();
		vm.Apply(circShader.ID);
	}
	glBindVertexArray(VAO_circ);
	glDrawElements(GL_TRIANGLES, inds_circ.size() + temporaryInds_circ.size(), GL_UNSIGNED_INT, 0);
	  
}

void MillingHeightMapRenderer::renderSegment(VariableManager& vm)
{ 
	if (useExternalShader)
	{
		externalShaderRect->use(); 
		vm.Apply(externalShaderRect->ID);
	}
	else
	{
		rectShader.use();
		vm.Apply(rectShader.ID);
	}
	glBindVertexArray(VAO_rect);
	int indInSegment = 6;
	glDrawElements(GL_TRIANGLES, indInSegment, GL_UNSIGNED_INT, (void*)(indInSegment * segmentToRender * sizeof(unsigned int)));

	if (useExternalShader)
	{
		externalShaderCirc->use();
		vm.Apply(externalShaderCirc->ID);
	}
	else
	{
		circShader.use();
		vm.Apply(circShader.ID);
	}
	glBindVertexArray(VAO_circ);
	indInSegment = 3 * circleDivisions;
	glDrawElements(GL_TRIANGLES, indInSegment, GL_UNSIGNED_INT, (void*)(indInSegment * segmentToRender * sizeof(unsigned int)));
}

void MillingHeightMapRenderer::Render(bool selected, VariableManager& vm)
{
	vm.SetVariable("color", glm::fvec4(1, 0, 0, 1));
	vm.SetVariable("radius", path.radius * sizeMultiplier.x);
	vm.SetVariable("flatMilling", path.flat);

	if (renderOneSegment)
		renderSegment(vm);
	else
		renderAll(vm);
}

void MillingHeightMapRenderer::SetRenderOneSegment(int i)
{
	renderOneSegment = true;
	segmentToRender = i;
}

void MillingHeightMapRenderer::SetRenderAll()
{
	renderOneSegment = false;
}

void MillingHeightMapRenderer::SetUseExternalShaders(std::shared_ptr<Shader> externalShaderRect, std::shared_ptr<Shader> externalShaderCirc)
{
	this->externalShaderRect = externalShaderRect;
	this->externalShaderCirc = externalShaderCirc;
	useExternalShader = true;
}

void MillingHeightMapRenderer::SetUseInternalShaders()
{
	useExternalShader = false;
}
