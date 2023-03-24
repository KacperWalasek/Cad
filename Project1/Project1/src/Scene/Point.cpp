#include "Point.h"
#include "imgui/imgui_stdlib.h"

Point::Point()
	:zero(), zero2(0), name("Point")
{	
    glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &zero, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int), &zero2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

std::string Point::getName() const
{
	return name;
}

Transform& Point::getTransform()
{
	return transform;
}

void Point::Render()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);
}

void Point::RenderGui()
{
	ImGui::Begin("Point");
	ImGui::InputText("Name", &name);
	transform.RenderGui();
	ImGui::End();
}
