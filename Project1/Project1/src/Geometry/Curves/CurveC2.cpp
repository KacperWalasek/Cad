#include "CurveC2.h"
#include "../../Scene/Scene.h"
#include <imgui/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>

Indexer CurveC2::indexer;

glm::fvec3 CurveC2::deBoor(float t, std::array<glm::fvec3, 4> points)
{
	std::array<glm::fvec3, 4> n{ glm::fvec3(0), glm::fvec3(0), glm::fvec3(0), glm::fvec3(1) };

	for (int i = 0; i < 3; i++)
		for (int j = 2 - i; j < 4; j++)
		{
			int ind = j - 2;
			int m = i + 1;
			if (j != 2 - i)
				n[j] *= (t - (ind - 1)) / (float)m;
			if (j != 3)
				n[j] = n[j] + n[j + 1] * (ind + m - t) / (float)m;
		}

	return n[0] * points[0] + n[1] * points[1] + n[2] * points[2] + n[3] * points[3];
}

glm::fvec3 CurveC2::deBoor(float t, std::array<glm::fvec3, 3> points)
{
	std::array<glm::fvec3, 3> n{ glm::fvec3(0), glm::fvec3(0), glm::fvec3(1) };

	for (int i = 0; i < 2; i++)
	{
		for (int j = 1 - i; j < 3; j++)
		{
			int ind = j - 1;
			int m = i + 1;
			if (j != 1 - i)
				n[j] *= (t - (ind - 1)) / (float)m;
			if (j != 2)
				n[j] = n[j] + n[j + 1] * (ind + m - t) / (float)m;

		}
	}

	return n[0] * points[0] + n[1] * points[1] + n[2] * points[2];
}

CurveC2::CurveC2()
	:addSelected(false),
	removeSelected(false),
	shader("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag"),
	deBoorShader("Shaders/DeBoor/deboor.vert", "Shaders/fragmentShader.frag"),
	selectedBezier(-1),
	showBezierChain(false), showBezierPoints(false), showDeboorChain(false)
{
	shader.Init();

	deBoorShader.Init();
	deBoorShader.loadShaderFile("Shaders/DeBoor/deboor.tesc", GL_TESS_CONTROL_SHADER);
	deBoorShader.loadShaderFile("Shaders/DeBoor/deboor.tese", GL_TESS_EVALUATION_SHADER);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &chainEBO);
	glGenBuffers(1, &curveEBO);
	glGenVertexArrays(1, &chainVAO);
	glGenVertexArrays(1, &curveVAO);
}

CurveC2::CurveC2(std::vector<std::shared_ptr<Point>> points)
	: CurveC2()
{
	this->points = points;
	name = "CurveC2-" + std::to_string(indexer.getNewIndex());
	UpdateMeshes();
}

CurveC2::CurveC2(nlohmann::json json, std::map<int, std::shared_ptr<Point>>& pointMap)
	: CurveC2()
{
	for (auto pi : json["deBoorPoints"])
		points.push_back(pointMap[pi["id"]]);
	if (json.contains("name"))
		name = json["name"];
	else
		name = "CurveC2-" + std::to_string(indexer.getNewIndex());
	UpdateMeshes();
}

void CurveC2::UpdateMeshes()
{
	UpdateBeziers();

	std::vector<float> vertices;
	std::vector<int> chainIndices;
	std::vector<int> curveIndices;

	for (int i = 0; i<points.size(); i++)
	{
		const glm::fvec4& v = points[i]->getTransform().location;
		vertices.push_back(v.x);
		vertices.push_back(v.y);
		vertices.push_back(v.z);

		if (i != 0)
		{
			chainIndices.push_back(i - 1);
			chainIndices.push_back(i);
		}
		if (i > 2)
		{
			curveIndices.push_back(i - 3);
			curveIndices.push_back(i - 2);
			curveIndices.push_back(i - 1);
			curveIndices.push_back(i);
		}
	}
	curveIndicesSize = curveIndices.size();
	chainIndicesSize = chainIndices.size();


	glBindVertexArray(curveVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if (vertices.size() != 0)
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curveEBO);
	if (curveIndices.size() != 0)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, curveIndices.size() * sizeof(int), &curveIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(chainVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chainEBO);

	if (chainIndices.size() != 0)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, chainIndices.size() * sizeof(int), &chainIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void CurveC2::UpdateBeziers()
{
	bezierChainMesh.vertices.clear();
	bezierChainMesh.indices.clear();
	bezierPoints.clear();
	for (int i = 1; i < (int)(points.size() - 2); i++)
	{
		// Bezier between p1 and p2
		glm::fvec4 db0 = points[i - 1]->getTransform().location;
		glm::fvec4 db1 = points[i]->getTransform().location;
		glm::fvec4 db2 = points[i + 1]->getTransform().location;
		glm::fvec4 db3 = points[i + 2]->getTransform().location;
		
		glm::fvec4 bl = db0 / 3.0f        + db1 * 2.0f / 3.0f;
		glm::fvec4 br = db2 * 2.0f / 3.0f + db3 / 3.0f;

		glm::fvec4 b1 = db1 * 2.0f / 3.0f + db2 / 3.0f;
		glm::fvec4 b2 = db1 / 3.0f        + db2 * 2.0f / 3.0f;
		glm::fvec4 b0 = (bl + b1) / 2.0f;
		glm::fvec4 b3 = (br + b2) / 2.0f;

		bezierPoints.emplace_back(b0);
		bezierPoints.emplace_back(b1);
		bezierPoints.emplace_back(b2);
		if (i == points.size() - 3)
			bezierPoints.emplace_back(b3);

	}
	for (int i = 0; i < bezierPoints.size(); i++)
	{
		glm::fvec4 v = bezierPoints[i].getTransform().location;
		bezierChainMesh.vertices.push_back(v.x);
		bezierChainMesh.vertices.push_back(v.y);
		bezierChainMesh.vertices.push_back(v.z);

		if (i != 0)
		{
			bezierChainMesh.indices.push_back(i - 1);
			bezierChainMesh.indices.push_back(i);
		}
	}
	bezierChainMesh.Update();
}

std::string CurveC2::getName() const
{
	return name;
}

void CurveC2::Render(bool selected, VariableManager& vm)
{
	glm::fvec4 selectionColor = vm.GetVariable<glm::fvec4>("color");
	shader.use();
	vm.SetVariable("color", glm::fvec4(1.0f, 0.0f, 1.0f, 1.0f));

	vm.Apply(shader.ID);
	if (showDeboorChain)
	{
		glBindVertexArray(chainVAO);
		glDrawElements(GL_LINES, chainIndicesSize, GL_UNSIGNED_INT, 0);
	}

	if (showBezierChain)
		bezierChainMesh.Render();

	if(showBezierPoints)
	{
		for (int i = 0; i<bezierPoints.size(); i++)
		{
			if (i == selectedBezier)
				vm.SetVariable("color", selectionColor);
			else
				vm.SetVariable("color", glm::fvec4(1.0f, 0.0f, 1.0f, 1.0f));
			vm.SetVariable("modelMtx", bezierPoints[i].getTransform().GetMatrix());

			vm.Apply(shader.ID);
			bezierPoints[i].Render(selected, vm);
		}
	}

	deBoorShader.use();

	vm.SetVariable("color", selectionColor);
	vm.SetVariable("modelMtx", glm::identity<glm::fmat4x4>());

	glBindVertexArray(curveVAO);
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	float baseDivision = 2.0f;
	for (int i = 0; i < baseDivision; i++)
	{
		vm.SetVariable("t0", i / baseDivision);
		vm.SetVariable("t1", (i + 1) / baseDivision);

		vm.Apply(deBoorShader.ID);
		glDrawElements(GL_PATCHES, curveIndicesSize, GL_UNSIGNED_INT, 0);
	}
}

bool CurveC2::RenderGui()
{
	ImGui::Begin("C2 Curve");
	ImGui::InputText("Name", &name);

	if (ImGui::Checkbox("Add on select", &addSelected))
		if (addSelected)
			removeSelected = false;
	if (ImGui::Checkbox("Remove on select", &removeSelected))
		if (removeSelected)
			addSelected = false;

	ImGui::Checkbox("Show deboor chain", &showDeboorChain);
	ImGui::Checkbox("Show bezier chain", &showBezierChain);
	ImGui::Checkbox("Show bezier points", &showBezierPoints);
	ImGui::End();
	return false;
}

void CurveC2::onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	auto curveAsSceneElem = std::find_if(scene.objects.begin(), scene.objects.end(),
		[this](const std::pair<std::shared_ptr<ISceneElement>, bool>& obj)
		{
			return obj.first.get() == this;
		});
	if (curveAsSceneElem != scene.objects.end() && curveAsSceneElem->second)
	{
		std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);
		if (p)
		{
			points.push_back(p);
			UpdateMeshes();
		}
	}
}

void CurveC2::onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);

	auto it = std::find(points.begin(), points.end(), elem);
	if (it != points.end())
	{
		points.erase(it);
		UpdateMeshes();
	}
}

void CurveC2::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);
	if (p)
	{
		auto pointIt = std::find(points.begin(), points.end(), elem);
		if (pointIt == points.end())
		{
			if (addSelected)
			{
				std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);
				if (p)
				{
					points.push_back(p);
					UpdateMeshes();
				}
			}
		}
		else
			if (removeSelected)
			{
				points.erase(pointIt);
				UpdateMeshes();
			}
		if (removeSelected || addSelected)
		{
			auto curveAsSceneElem = std::find_if(scene.objects.begin(), scene.objects.end(),
				[this](const std::pair<std::shared_ptr<ISceneElement>, bool>& obj)
				{
					return obj.first.get() == this;
				});
			scene.Select(curveAsSceneElem->first);
		}
	}
}

void CurveC2::onMove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	if (std::find(points.begin(), points.end(), elem) != points.end())
		UpdateMeshes();
}

std::tuple<int, float> CurveC2::getClickIndex(Camera& camera, float x, float y) const
{
	int bestIndex = -1;
	float bestZ = 1;
	for (int i = 0; i < bezierPoints.size(); i++)
	{
		glm::fvec4 screenPosition = camera.GetProjectionMatrix() * camera.GetViewMatrix() * bezierPoints[i].getTransform().GetMatrix() * glm::fvec4(0,0,0,1);
		screenPosition /= screenPosition.w;
		float dist2 = pow(x - screenPosition.x, 2) + pow(y - screenPosition.y, 2);
		if (dist2 < 0.0001f && bestZ>screenPosition.z)
		{
			bestZ = screenPosition.z;
			bestIndex = i;
		}
	}
	return std::make_tuple(bestIndex, bestZ);
}

std::tuple<bool, float> CurveC2::InClickRange(Camera& camera, float x, float y) const
{
	auto [index, z] = getClickIndex(camera,x,y);
	return std::make_tuple(index>-1, z);
}

bool CurveC2::Click(Scene& scene, Camera& camera, float x, float y)
{
	auto [index, z] = getClickIndex(camera, x, y);
	selectedBezier = index;
	return true;
}

bool CurveC2::Translate(glm::fvec4 translation)
{
	if (selectedBezier < 0)
		return false;
	glm::fvec4 oldLoc = bezierPoints[selectedBezier].getTransform().location;
	bezierPoints[selectedBezier].getTransform().location = movementStartPosition + translation;
	glm::fvec4 dif = bezierPoints[selectedBezier].getTransform().location- oldLoc;
	int leftInd = floor(selectedBezier / 3) + 1;
	int rightInd = leftInd + 1;
	switch (selectedBezier % 3)
	{
	case 0:
		points[leftInd]->getTransform().location += 2.0f*dif;

		points[leftInd-1]->getTransform().location -= dif;
		points[rightInd]->getTransform().location -= dif;
		break;
	case 1:
		{
			glm::fvec4 prevPos = bezierPoints[selectedBezier - 1].getTransform().location;
			glm::fvec4 currPos = movementStartPosition + translation;
			glm::fvec4 nextPos = bezierPoints[selectedBezier + 1].getTransform().location;
			glm::fvec4 leftDeboor = 2.0f * currPos - nextPos;
			points[leftInd]->getTransform().location = leftDeboor;
			points[rightInd]->getTransform().location = 2.0f * nextPos - currPos;
			
			glm::fvec4 prevprevPos = 2.0f * prevPos - currPos;
			points[leftInd - 1]->getTransform().location = prevprevPos - 2.0f * (leftDeboor - prevprevPos);
			
		}
		break;
	case 2:
		{
			glm::fvec4 prevPos = bezierPoints[selectedBezier - 1].getTransform().location;
			glm::fvec4 currPos = movementStartPosition + translation;
			glm::fvec4 nextPos = bezierPoints[selectedBezier + 1].getTransform().location;
			glm::fvec4 rightDeboor = 2.0f * currPos - prevPos;
			points[leftInd]->getTransform().location = 2.0f * prevPos - currPos;
			points[rightInd]->getTransform().location = rightDeboor;
			
			glm::fvec4 nextnextPos = 2.0f * nextPos - currPos;
			points[rightInd + 1]->getTransform().location = nextnextPos + 2.0f * (nextnextPos - rightDeboor);
		}
		break;
	}
	UpdateMeshes();
	return true;
}

bool CurveC2::Rotate(glm::fvec4 rotation)
{
	return false;
}

bool CurveC2::Scale(glm::fvec4 scale)
{
	return false;
}

void CurveC2::StartMove()
{
	if (selectedBezier >= 0)
		movementStartPosition = bezierPoints[selectedBezier].getTransform().location;
}

void CurveC2::Unclick()
{
	selectedBezier = -1;
}

nlohmann::json CurveC2::Serialize(Scene& scene, Indexer& indexer, std::map<int, int>& pointIndexMap) const
{
	std::vector<nlohmann::json> pointIds;
	for (auto& p : points)
		pointIds.push_back({ {"id",pointIndexMap.find(p->getId())->second } });

	return {
		{"objectType", "bezierC2"},
		{"id", indexer.getNewIndex()},
		{"name", name},
		{"deBoorPoints", pointIds }
	};
}

void CurveC2::onCollapse(Scene& scene, std::vector<std::shared_ptr<Point>>& collapsed, std::shared_ptr<Point> result)
{
	for (int i = 0; i < points.size(); i++)
		for (auto& c : collapsed)
			if (points[i] == c)
				points[i] = result;
	UpdateMeshes();
}
