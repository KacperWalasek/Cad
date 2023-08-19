#include "Point.h"
#include "imgui/imgui_stdlib.h"

Indexer Point::indexer;
int Point::getId() const
{
	return id;
}
Point::Point()
	: id(indexer.getNewIndex()), name("Point-" + std::to_string(id)), zero(), zero2(0)
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

Point::Point(nlohmann::json json)
	:Point()
{
	transform.location.x = json["position"]["x"];
	transform.location.y = json["position"]["y"];
	transform.location.z = json["position"]["z"];
}


Point::Point(glm::fvec4 position)
	:Point()
{
	transform.location = position;
}

std::string Point::getName() const
{
	return name;
}

Transform& Point::getTransform()
{
	return transform;
}

const Transform& Point::getTransform() const
{
	return transform;
}

void Point::Render(bool selected, VariableManager& vm)
{
	glBindVertexArray(VAO);
	glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);
}

bool Point::RenderGui()
{
	ImGui::Begin("Point");
	ImGui::InputText("Name", &name);
	if (transform.RenderGui())
	{
		ImGui::End();
		return true;
	}
	ImGui::End();
	return false;
}

std::tuple<bool, float> Point::InClickRange(Camera& camera,float x, float y) const
{
	glm::fvec4 screenPosition = camera.GetProjectionMatrix() * camera.GetViewMatrix() * transform.GetMatrix() * glm::fvec4(0, 0, 0, 1);
	screenPosition /= screenPosition.w;
	float dist2 = pow(x - screenPosition.x, 2) + pow(y - screenPosition.y, 2);
	if (dist2 < 0.0001f)
		return std::make_tuple( true, screenPosition.z );
	return std::make_tuple(false, 0 );
}

bool Point::Click(Scene& scene, Camera& camera, float x, float y)
{
	return true;
}

void Point::Unclick()
{
}

bool Point::canBeDeleted() const
{
	for (auto& o : po)
		if (!o.lock()->CanChildBeDeleted())
			return false;
	return true;
}

bool Point::canBeMoved() const
{
	for (auto& o : po)
	{
		if (!o.lock()->CanChildBeMoved())
			return false;
	}
	return true;
}


nlohmann::json Point::Serialize(Scene& scene, Indexer& indexer, std::map<int, int>& pointIndexMap) const
{
	int jsonId = indexer.getNewIndex();
	pointIndexMap.insert({id,jsonId});

	nlohmann::json p = {
		{"id", jsonId},
		{ "position", {
			{"x", transform.location.x},
			{"y", transform.location.y},
			{"z", transform.location.z}
		}}
	};

	return p;
}
