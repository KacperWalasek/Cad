#include "SurfaceC2.h"
#include "../Curves/CurveC2.h"
#include <array>

Indexer SurfaceC2::indexer;

void SurfaceC2::updateMeshes()
{
	std::vector<int> inds;
	std::vector<float> vertices;

	int rowSize = countX + 3;
	for (int i = 0; i < points.size() || i < positions.size(); i++)
	{
		float u = ((i % rowSize) - 1) / (float)countX;
		float v = (floorf(i / rowSize) - 1) / (float)countY;

		auto& pos = points.size() == 0 ? positions[i] : points[i]->getTransform().location;
		vertices.insert(vertices.end(), {
			pos.x,pos.y,pos.z,
			u,v
			});
	}

	for(int i = 0; i < countY; i++)
		for (int j = 0; j < countX; j++)
		{
			int offset = i * rowSize + j;
			for (int y = 0; y < 4; y++)
				for(int x = 0; x < 4; x++)
				{
					inds.push_back(offset + y * rowSize + x);
				}
		}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	indicesSize = inds.size();
}

void SurfaceC2::CreatePointsFlat()
{
	float distX = sizeX / (float)countX;
	float distY = sizeY / (float)countY;
	glm::fvec4 middle(sizeX / 2.0f + distX, 0, sizeY / 2.0f + distY,0);
	for (int i = 0; i < countY + 3; i++)
		for (int j = 0; j < countX + 3; j++)
			positions.push_back(pos - middle + glm::fvec4(j * distX, 0.0f, i * distY, 0.0f));
}

void SurfaceC2::CreatePointsCylinder()
{
	float distY = sizeY / (countY * 1);
	glm::fvec4 middle(0, 0, sizeY / 2.0f + distY, 0);

	float da = 2 * 3.14 / countX;
	float R = sizeX * (3-cosf(da)) / 2.0f;

	for(int h = 0; h < countY + 3; h++)
		for (int a = 0; a < countX + 3; a++)
		{
			float angle1 = 2 * 3.14 * a / countX;
			positions.push_back(pos - middle + glm::fvec4( R * cosf(angle1) , R * sinf(angle1) , h * distY, 0.0f ));

		}
}

int SurfaceC2::pointIndex(int sX, int sY, int pX, int pY) const
{
	int rowSize = countX + 3;
	int offset = sY * rowSize + sX;
		
	return offset + pY * rowSize + pX;
		
}

SurfaceC2::SurfaceC2(glm::fvec4 pos, int countX, int countY, float sizeX, float sizeY, bool cylinder)
	: SurfaceC2()
{
	name = "SurfaceC2-" + std::to_string(indexer.getNewIndex());
	division[0] = 4;
	division[1] = 4;
	this->countX = countX;
	this->countY = countY;
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->cylinder = cylinder;
	this->pos = pos;

	Recalculate();
}

SurfaceC2::SurfaceC2(nlohmann::json json, std::map<int, std::shared_ptr<Point>>& pointMap)
	: SurfaceC2()
{
	if (json.contains("name"))
		name = json["name"];
	else
		name = "SurfaceC2-" + std::to_string(indexer.getNewIndex());
	countX = json["size"]["x"];
	countY = json["size"]["y"];
	cylinder = json["parameterWrapped"]["u"];

	std::vector<nlohmann::json> patches = json["patches"];
	points = std::vector<std::shared_ptr<Point>>((3 + countX) * (3 + countY));
	
	division[0] = patches[0]["samples"]["x"];
	division[1] = patches[0]["samples"]["y"];

	float rowSize = 3 + countX;
	for (int i = 0; i < countY; i++)
		for (int j = 0; j < countX; j++)
		{
			auto patch = patches[i * countX + j]["controlPoints"];
			if (i == 0 && j == 0)
			{
				for (int y = 0; y < 3; y++)
					for (int x = 0; x < 3; x++)
						points[y * rowSize + x] = pointMap[patch[y * 4 + x]["id"]];
			}
			if (j == 0)
				for (int x = 0; x < 3; x++)
					points[(3 + i) * rowSize + x] = pointMap[patch[12 + x]["id"]];
			if (i == 0)
				for (int y = 0; y < 3; y++)
					points[y * rowSize + (3 + j)] = pointMap[patch[3 + y * 4]["id"]];

			points[(3 + i) * rowSize + (3 + j)] = pointMap[patch[15]["id"]];
		}

	updateMeshes();
}
SurfaceC2::SurfaceC2()
	: tessShader("Shaders/Surface/surfaceC0.vert", "Shaders/uv.frag"),
	chainShader("Shaders/Surface/surfaceC0.vert", "Shaders/fragmentShader.frag"),
	shouldReload(false),
	showChain(false)
{

	tessShader.Init();
	tessShader.loadShaderFile("Shaders/Surface/surfaceC2.tesc", GL_TESS_CONTROL_SHADER);
	tessShader.loadShaderFile("Shaders/Surface/surfaceC2.tese", GL_TESS_EVALUATION_SHADER);
	chainShader.Init();
	chainShader.loadShaderFile("Shaders/Surface/surfaceChain.tesc", GL_TESS_CONTROL_SHADER);
	chainShader.loadShaderFile("Shaders/Surface/surfaceChain.tese", GL_TESS_EVALUATION_SHADER);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);

}

std::string SurfaceC2::getName() const
{
	return name;
}

void SurfaceC2::Render(bool selected, VariableManager& vm)
{
	if (shouldReload)
	{
		updateMeshes();
		shouldReload = false;
	}

	tessShader.use();
	glBindVertexArray(VAO);
	glPatchParameteri(GL_PATCH_VERTICES, 16);

	int enabledIntCount = 0;
	std::vector<bool> revInt;
	for (int i = 0; i < intersections.size(); i++)
	{
		auto intersection = intersections[i].lock();
		if (!intersectEnabled[i])
			continue;

		revInt.push_back(intersectReversed[i]);
		enabledIntCount++;
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, intersectionTextures[i]);
	}

	std::vector<int> texInds(enabledIntCount);
	std::iota(texInds.begin(), texInds.end(), 0);

	vm.SetVariable("interesectTex", texInds);
	vm.SetVariable("interesectCount", enabledIntCount);
	vm.SetVariable("reverseIntersect", revInt);

	vm.SetVariable("divisionU", division[0]);
	vm.SetVariable("divisionV", division[1]);

	vm.SetVariable("reverse", false);
	vm.Apply(tessShader.ID);
	glDrawElements(GL_PATCHES, indicesSize, GL_UNSIGNED_INT, 0);

	vm.SetVariable("reverse", true);
	vm.Apply(tessShader.ID);
	glDrawElements(GL_PATCHES, indicesSize, GL_UNSIGNED_INT, 0);

	if (showChain)
	{
		vm.SetVariable("color", glm::fvec4(1.0f, 0.0f, 1.0f, 1.0f));
		chainShader.use();
		vm.SetVariable("reverse", false);
		vm.Apply(chainShader.ID);
		glDrawElements(GL_PATCHES, indicesSize, GL_UNSIGNED_INT, 0);

		vm.SetVariable("reverse", true);
		vm.Apply(chainShader.ID);
		glDrawElements(GL_PATCHES, indicesSize, GL_UNSIGNED_INT, 0);
	}
}

void SurfaceC2::onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
}

void SurfaceC2::onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	if (elem.get() == this)
		for (auto& p : points)
		{
			std::erase_if(p->po, [this](const std::weak_ptr<IOwner>& o) {
				return o.lock().get() == this;
				});
		}
}

void SurfaceC2::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
}

void SurfaceC2::onMove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	if (std::find(points.begin(), points.end(), elem) != points.end())
		shouldReload = true;
}

bool SurfaceC2::CanChildBeDeleted() const
{
	return false;
}

bool SurfaceC2::RenderGui()
{
	ImGui::Begin("Surface C2");
	if (ImGui::InputInt2("Division", division))
	{
		division[0] = glm::clamp(division[0], 2, 1000);
		division[1] = glm::clamp(division[1], 2, 1000);
		shouldReload = true;
	}
	ImGui::Checkbox("Show Chain", &showChain);
	
	for (int i = 0; i < intersections.size(); i++)
	{
		bool reversed = intersectReversed[i];
		bool enabled = intersectEnabled[i];
		ImGui::Text(intersections[i].lock()->getName().c_str());
		if (ImGui::Checkbox(("Enabled"+std::to_string(i)).c_str(), &enabled))
			intersectEnabled[i] = enabled;
		if (ImGui::Checkbox(("Reversed"+ std::to_string(i)).c_str(), &reversed))
			intersectReversed[i] = reversed;
	}

	ImGui::End();
	return false;
}

void SurfaceC2::SelectAll(Scene& scene) const
{
	for (auto& p : points)
	{
		auto it = std::find_if(scene.objects.begin(), scene.objects.end(),
			[&p](const std::pair<std::shared_ptr<ISceneElement>, bool>& el) {
				return el.first.get() == p.get();
			});
		if (it != scene.objects.end())
			it->second = true;
	}
}

void SurfaceC2::Recalculate()
{
	positions.clear();
	if (cylinder)
		CreatePointsCylinder();
	else
		CreatePointsFlat();

	updateMeshes();
}

void SurfaceC2::CreateControlPoints()
{
	if(cylinder)
		for (int h = 0; h < countY + 3; h++)
		{
			for (int a = 0; a < countX; a++)
				points.push_back(std::make_shared<Point>(positions[h * (countX+3) + a]));
			for (int i = 0; i < 3; i++)
				points.push_back(points[h * (countX + 3) + i]);
		
		}
	else
		for (auto& p : positions)
		{
			points.push_back(std::make_shared<Point>(p));
		}
	positions.clear();
}

nlohmann::json SurfaceC2::Serialize(Scene& scene, Indexer& indexer, std::map<int, int>& pointIndexMap) const
{
	std::vector<nlohmann::json> patches;

	for (int i = 0; i < countY; i++)
		for (int j = 0; j < countX; j++)
		{
			std::vector<nlohmann::json>  patchPoints;
			int rowSize = countX + 3;
			int offset = i * rowSize + j;

			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{
					patchPoints.push_back({ {"id",pointIndexMap.find(points[offset + y * rowSize + x]->getId())->second}});
				}
			
			patches.push_back({
				{"objectType","bezierPatchC2"},
				{"id", indexer.getNewIndex()},
				{"controlPoints", patchPoints},
				{"samples",
					{
						{"x", division[0]},
						{"y", division[1]}
					}
				}
				});
		}

	return {
			{"objectType", "bezierSurfaceC2"},
			{"id", indexer.getNewIndex()},
			{"name", name},
			{"patches", patches},
			{"parameterWrapped",{
				{"u", cylinder},
				{"v", false}
			}},
			{"size",
				{
					{"x", countX},
					{"y", countY}
				}
			}
	};
}

void SurfaceC2::onCollapse(Scene& scene, std::vector<std::shared_ptr<Point>>& collapsed, std::shared_ptr<Point> result)
{
	for (int i = 0; i < points.size(); i++)
		for (auto& c : collapsed)
			if (points[i] == c)
			{
				points[i] = result;
				shouldReload = true;
			}
}

glm::fvec3 SurfaceC2::f(float u, float v) const
{
	int sx = glm::min((int)floor(countX * u), countX - 1);
	int sy = glm::min((int)floor(countY * v), countY - 1);

	float unitX = 1.0f / (float)countX;
	float unitY = 1.0f / (float)countY;

	u = (u - unitX * sx) * countX;
	v = (v - unitY * sy) * countY;

	std::array<glm::fvec3, 4> subPoints;
	std::array<glm::fvec3, 4> deBoorPoints;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			deBoorPoints[j] = points[pointIndex(sx, sy, i, j)]->getTransform().location;

		subPoints[i] = CurveC2::deBoor(v, deBoorPoints);
	}

	return CurveC2::deBoor(u, subPoints);

}

glm::fvec3 SurfaceC2::dfdu(float u, float v) const
{
	int sx = glm::min((int)floor(countX * u), countX - 1);
	int sy = glm::min((int)floor(countY * v), countY - 1);

	float unitX = 1.0f / (float)countX;
	float unitY = 1.0f / (float)countY;

	u = (u - unitX * sx) * countX;
	v = (v - unitY * sy) * countY;

	std::array<glm::fvec3, 4> subPoints;
	std::array<glm::fvec3, 4> deBoorPoints;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			deBoorPoints[j] = points[pointIndex(sx, sy, i, j)]->getTransform().location;

		subPoints[i] = CurveC2::deBoor(v, deBoorPoints);
	}

	std::array<glm::fvec3, 3> derivative;
	for (int i = 0; i < 3; i++)
		derivative[i] = subPoints[i + 1] - subPoints[i];

	return CurveC2::deBoor(u, derivative) * (float)countX;


}

glm::fvec3 SurfaceC2::dfdv(float u, float v) const
{
	int sx = glm::min((int)floor(countX * u), countX - 1);
	int sy = glm::min((int)floor(countY * v), countY - 1);

	float unitX = 1.0f / (float)countX;
	float unitY = 1.0f / (float)countY;

	u = (u - unitX * sx) * countX;
	v = (v - unitY * sy) * countY;

	std::array<glm::fvec3, 4> subPoints;
	std::array<glm::fvec3, 4> deBoorPoints;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			deBoorPoints[j] = points[pointIndex(sx, sy, j, i)]->getTransform().location;

		subPoints[i] = CurveC2::deBoor(u, deBoorPoints);
	}

	std::array<glm::fvec3, 3> derivative;
	for (int i = 0; i < 3; i++)
		derivative[i] = subPoints[i + 1] - subPoints[i];

	return CurveC2::deBoor(v, derivative) * (float)countY;
}

bool SurfaceC2::wrappedU()
{
	return cylinder;
}

bool SurfaceC2::wrappedV()
{
	return false;
}

void SurfaceC2::acceptIntersection(std::weak_ptr<Intersection> intersection)
{
	auto intLock = intersection.lock();

	intersections.push_back(intersection);

	if (this == intLock->s1.get())
		intersectionTextures.push_back(intLock->uvS1Tex);

	if (this == intLock->s2.get())
		intersectionTextures.push_back(intLock->uvS2Tex);

	intersectEnabled.push_back(false);
	intersectReversed.push_back(false);

}

void SurfaceC2::removeIntersection(std::weak_ptr<Intersection> intersection)
{
	auto intLock = intersection.lock();

	std::erase_if(intersections, [intLock](const std::weak_ptr<Intersection>& i) {
		return i.lock().get() == intLock.get();
		});
	std::erase_if(intersectionTextures, [&intLock](const unsigned int& i) {
		return i == intLock->uvS1Tex || i == intLock->uvS2Tex;
		});
}
