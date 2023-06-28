#include "BrokenLine.h"

void BrokenLine::updateMesh(bool cullLong)
{
	std::vector<float> vert;
	std::vector<int> inds;

	for (auto& p : points)
		vert.insert(vert.end(), {
			p.x,p.y,p.z
		});

	for (int i = 0; i < points.size()-1; i++)
	{
		if(!cullLong || glm::length(points[i+1] - points[i]) < 0.5f)
		{
			inds.push_back(i);
			inds.push_back(i + 1);
		}
	}

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void BrokenLine::setPoints(std::vector<glm::fvec3> points, bool cullLong)
{
	this->points = points;
	updateMesh(cullLong);
}

BrokenLine::BrokenLine()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
}

void BrokenLine::Render(bool selected, VariableManager& vm)
{
	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, points.size() * 2 - 2, GL_UNSIGNED_INT, 0);
}
