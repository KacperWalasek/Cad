#include "BrokenLine.h"

void BrokenLine::updateMesh(bool cullLong)
{
	std::vector<float> vert;
	std::vector<int> inds;


	int culled = 0;
	for (int i = 0; i < points.size() - 1; i++)
	{
		vert.insert(vert.end(), {
			points[i].x,points[i].y,points[i].z
			});

		inds.push_back(i + 2 * culled);
		inds.push_back(i + 2 * culled + 1);

		glm::fvec3 dir = points[i + 1] - points[i];
		if (cullLong && (fabsf(dir.x)>0.5 || fabsf(dir.y) > 0.5))
		{
			glm::fvec2 dir1 = fabsf(dir.x) > fabsf(dir.y) ? glm::fvec2{-dir.x, dir.y} : glm::fvec2{dir.x, -dir.y};
			auto p1 = cull(points[i], dir1);
			auto p2 = cull(points[i + 1], -dir1);

			vert.insert(vert.end(), {
				p1.x,p1.y,0,
				p2.x,p2.y,0
				});

			inds.push_back(i + 2 * culled + 2);
			inds.push_back(i + 2 * culled + 3);

			culled++;
		}

	}
	vert.insert(vert.end(), {
		points.rbegin()->x,points.rbegin()->y,points.rbegin()->z
		});

	indsSize = inds.size();
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

glm::fvec2 BrokenLine::cull(glm::fvec2 x, glm::fvec2 dir)
{
	auto maxACond1 = (glm::fvec2(1.0f) - x) / dir;
	auto maxACond2 = -x / dir;
	std::vector<float> max = {
		maxACond1.x, maxACond1.y,
		maxACond2.x, maxACond2.y };

	float a = 10;
	for (int i = 0; i < 4; i++)
		if (max[i] >= 0 && a > max[i])
			a = max[i];

	return x + dir * a;
	
}

void BrokenLine::setPoints(std::vector<glm::fvec3> points, bool cullLong)
{
	this->points = points;
	updateMesh(cullLong);
}

std::vector<glm::fvec3> BrokenLine::getPoints() const
{
	return points;
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
	glDrawElements(GL_LINES, indsSize, GL_UNSIGNED_INT, 0);
}
