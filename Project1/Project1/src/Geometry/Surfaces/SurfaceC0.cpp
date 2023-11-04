#include "SurfaceC0.h"
#include "../Curves/BezierCurve.h"

Indexer SurfaceC0::indexer;

void SurfaceC0::updateMeshes()
{
	std::vector<int> inds;
	std::vector<float> vertices;

	int rowSize = countX * 3 + 1;
	for(int i =0; i<points.size() || i < positions.size(); i++) 
	{
		float u = (i % rowSize) / (float)(rowSize - 1);
		float v = floorf(i/rowSize) / (float)(countY * 3);
		auto& pos = points.size() == 0 ? positions[i] : points[i]->getTransform().location;
		vertices.insert(vertices.end(), {
			pos.x,pos.y,pos.z,
			u,v
			});
	}

	for (int i = 0; i < countY; i++)
		for (int j = 0; j < countX; j++)
		{
			int offset = 3 * i * rowSize + 3 * j;
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{
					inds.push_back((offset + y * rowSize + x) );
				}
		}


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	indicesSize = inds.size();
}

void SurfaceC0::CreatePointsFlat()
{
	float distX = sizeX / (countX * 3);
	float distY = sizeY / (countY * 3);
	glm::fvec4 middle(sizeX / 2.0f, 0, sizeY / 2.0f, 0);
	for (int i = 0; i < 1 + countY * 3; i++)
		for (int j = 0; j < 1 + countX * 3; j++)
			positions.push_back(pos - middle + glm::fvec4(j * distX, 0.0f, i * distY,0.0f));
}

void SurfaceC0::CreatePointsCylinder()
{
	float distY = sizeY / (countY * 3);
	glm::fvec4 middle(0, 0, sizeY / 2.0f, 0);
	for (int h = 0; h < 1 + countY * 3; h++)
	{
		int offset = positions.size();
		for (int a = 0; a < countX; a++)
		{
			float angle1 = 2 * 3.14 * a / countX;
			float angle2 = 2 * 3.14 * (a + 1) / countX;

			glm::fvec4 l1 = { sizeX * cosf(angle1) , sizeX * sinf(angle1) , h * distY, 0.0f };
			glm::fvec4 l2 = { sizeX * cosf(angle2) , sizeX * sinf(angle2) , h * distY, 0.0f };

			glm::fvec4 s1 = glm::fvec4(-sinf(angle1), cosf(angle1), 0.0f, 0.0f);
			glm::fvec4 s2 = glm::fvec4(-sinf(angle2), cosf(angle2), 0.0f, 0.0f);

			float cosA = glm::dot(glm::normalize(s1), glm::normalize(l2 - l1));
			float mult = glm::length(l1 - l2) / 4.0f / cosA;

			positions.push_back(pos - middle + l1);
			positions.push_back(pos - middle + l1 + mult * glm::fvec4(-sinf(angle1), cosf(angle1), 0.0f, 0.0f));
			positions.push_back(pos - middle + l2 - mult * glm::fvec4(-sinf(angle2), cosf(angle2), 0.0f, 0.0f));
		}
		positions.push_back(positions[offset]);
	}
}

void SurfaceC0::CreateSmiglo()
{
	for (int h = 0; h < 1 + countY * 3; h++)
		for (int a = 0; a < countX; a++)
		{
			float angle1 = 2 * 3.14 * a / countX;
			float angle2 = 2 * 3.14 * (a + 1) / countX;

			glm::fvec4 l1 = { 10 * cosf(angle1) , 10 * sinf(angle1) , h * 10, 1.0f };
			glm::fvec4 l2 = { 10 * cosf(angle2) , 10 * sinf(angle2) , h * 10, 1.0f };

			glm::fvec4 s1 = glm::fvec4(-sinf(angle1), cosf(angle1), 0.0f, 0.0f);
			glm::fvec4 s2 = glm::fvec4(-sinf(angle2), cosf(angle2), 0.0f, 0.0f);

			float cosA = glm::dot(l1, l2);
			float mult = glm::length(l1 - l2) / 3 * cosA;

			positions.push_back(l1);
			positions.push_back(l1 + mult * glm::fvec4(-sinf(angle1), cosf(angle1), 0.0f, 0.0f));
			positions.push_back(l2 - mult * glm::fvec4(-sinf(angle2), cosf(angle2), 0.0f, 0.0f));

		}
}

int SurfaceC0::pointIndex(int sX, int sY, int pX, int pY) const
{
	int rowSize = countX * 3 + 1;
	int offset = 3 * sY * rowSize + 3 * sX;
	return offset + pY * rowSize + pX;
}

SurfaceC0::SurfaceC0(glm::fvec4 pos, int countX, int countY, float sizeX, float sizeY, bool cylinder)
	: SurfaceC0()
{
	name = "SurfaceC0-" + std::to_string(indexer.getNewIndex());
	this->countX = countX;
	this->countY = countY;
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->cylinder = cylinder;
	this->pos = pos;

	Recalculate();
}

SurfaceC0::SurfaceC0(nlohmann::json json, std::map<int, std::shared_ptr<Point>>& pointMap)
	: SurfaceC0()
{
	if(json.contains("name"))
		name = json["name"];
	else
		name = "SurfaceC0-" + std::to_string(indexer.getNewIndex());

	countX = json["size"]["x"];
	countY = json["size"]["y"];
	cylinder = json["parameterWrapped"]["u"];

	std::vector<nlohmann::json> patches = json["patches"];
	points = std::vector<std::shared_ptr<Point>>( (1 + 3 * countX) * (1 + 3 * countY));

	for (int i = 0; i < countY; i++)
		for (int j = 0; j < countX; j++)
		{
			auto path = patches[i * countX + j]["controlPoints"];
			int rowSize = countX * 3 + 1;
			int offset = 3 * i * rowSize + 3 * j;
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{
					points[offset + y * rowSize + x] = pointMap[path[y * 4 + x]["id"]];
				}
		}
	updateMeshes();
}
SurfaceC0::SurfaceC0()
	:tessShader("Shaders/Surface/surfaceC0.vert", "Shaders/uv.frag"),
	chainShader("Shaders/Surface/surfaceC0.vert", "Shaders/fragmentShader.frag"),
	shouldReload(false),
	showChain(false)
{
	division[0] = 4;
	division[1] = 4;

	tessShader.Init();
	tessShader.loadShaderFile("Shaders/Surface/surfaceC0.tesc", GL_TESS_CONTROL_SHADER);
	tessShader.loadShaderFile("Shaders/Surface/surfaceC0.tese", GL_TESS_EVALUATION_SHADER);
	chainShader.Init();
	chainShader.loadShaderFile("Shaders/Surface/surfaceChain.tesc", GL_TESS_CONTROL_SHADER);
	chainShader.loadShaderFile("Shaders/Surface/surfaceChain.tese", GL_TESS_EVALUATION_SHADER);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
}

std::string SurfaceC0::getName() const
{
	return name;
}

void SurfaceC0::Render(bool selected, VariableManager& vm)
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

void SurfaceC0::onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
}

void SurfaceC0::onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	if (elem.get() == this)
	{
		for (auto& p : points)
		{
			std::erase_if(p->po, [this](const std::weak_ptr<IOwner>& o) {
				return o.lock().get() == this;
				});
		}
	}
}

void SurfaceC0::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
}

void SurfaceC0::onMove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
}

bool SurfaceC0::CanChildBeDeleted() const
{
	return false;
}

bool SurfaceC0::RenderGui()
{
	ImGui::Begin("Surface C0");
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
		if (ImGui::Checkbox(("Enabled" + std::to_string(i)).c_str(), &enabled))
			intersectEnabled[i] = enabled;
		if (ImGui::Checkbox(("Reversed" + std::to_string(i)).c_str(), &reversed))
			intersectReversed[i] = reversed;
	}
	ImGui::End();
	return false;
}

void SurfaceC0::SelectAll(Scene& scene) const
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

void SurfaceC0::Recalculate()
{
	positions.clear();
	if (cylinder)
		CreatePointsCylinder();
	else
		CreatePointsFlat();

	updateMeshes();
}

void SurfaceC0::CreateControlPoints()
{
	if (cylinder)
		for (int h = 0; h < 1 + countY * 3; h++)
		{
			for (int a = 0; a < countX*3; a++)
				points.push_back(std::make_shared<Point>(positions[h * (countX*3+1) + a]));
			points.push_back(points[h * (countX*3 + 1)]);
		}
	else
		for (auto& p : positions)
		{
			points.push_back(std::make_shared<Point>(p));
		}
	positions.clear();
}

nlohmann::json SurfaceC0::Serialize(Scene& scene, Indexer& indexer, std::map<int, int>& pointIndexMap) const
{
	std::vector<nlohmann::json> patches;
	for (int i = 0; i < countY; i++)
		for (int j = 0; j < countX; j++)
		{
			std::vector<nlohmann::json> patchPoints;
			int rowSize = countX * 3 + 1;
			int offset = 3 * i * rowSize + 3 * j;
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{
					patchPoints.push_back({ {"id",pointIndexMap.find(points[offset + y * rowSize + x]->getId())->second}});
				}
			patches.push_back({
				{"objectType","bezierPatchC0"},
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
			{"objectType", "bezierSurfaceC0"},
			{"id", indexer.getNewIndex()},
			{"name", name},
			{"patches", patches},
			{"parameterWrapped",
				{
					{"u", cylinder},
					{"v", false}
				}
			},
			{"size",
				{
					{"x", countX},
					{"y", countY}
				}
			}
		};
}

void SurfaceC0::onCollapse(Scene& scene, std::vector<std::shared_ptr<Point>>& collapsed, std::shared_ptr<Point> result)
{
	for (int i = 0; i < points.size(); i++)
		for (auto& c : collapsed)
			if (points[i].get() == c.get())
			{
				points[i] = result;
				shouldReload = true;
			}
}

bool SurfaceC0::canBeDeleted() const
{
	for (auto& o : po)
		if (!o.lock()->CanChildBeDeleted())
			return false;
	return true;
}

glm::fvec3 SurfaceC0::f(float u, float v) const
{
	int sx = glm::min((int)floor(countX * u), countX - 1);
	int sy = glm::min((int)floor(countY * v), countY - 1);
	
	float unitX = 1.0f / (float)countX;
	float unitY = 1.0f / (float)countY;

	u = (u - unitX * sx) * countX;
	v = (v - unitY * sy) * countY;

	std::array<glm::fvec3, 4> subPoints;
	std::array<glm::fvec3, 4> deCastiljeuPoints;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			deCastiljeuPoints[j] = points[pointIndex(sx, sy, i, j)]->getTransform().location;

		subPoints[i] = BezierCurve::deCastljeu(v, deCastiljeuPoints);
	}

	return BezierCurve::deCastljeu(u, subPoints);
}

glm::fvec3 SurfaceC0::dfdu(float u, float v) const
{
	int sx = glm::min((int)floor(countX * u), countX - 1);
	int sy = glm::min((int)floor(countY * v), countY - 1);

	float unitX = 1.0f / (float)countX;
	float unitY = 1.0f / (float)countY;

	u = (u - unitX * sx) * countX;
	v = (v - unitY * sy) * countY;

	std::array<glm::fvec3, 4> subPoints;
	std::array<glm::fvec3, 4> deCastiljeuPoints;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			deCastiljeuPoints[j] = points[pointIndex(sx, sy, i, j)]->getTransform().location;

		subPoints[i] = BezierCurve::deCastljeu(v, deCastiljeuPoints);
	}

	std::array<glm::fvec3, 3> derivative;

	for (int i = 0; i < 3; i++)
		derivative[i] = 3.0f * (subPoints[i + 1] - subPoints[i]);


	return BezierCurve::deCastljeu(u, derivative) * (float)countX;
}

glm::fvec3 SurfaceC0::dfdv(float u, float v) const
{
	int sx = glm::min((int)floor(countX * u), countX - 1);
	int sy = glm::min((int)floor(countY * v), countY - 1);

	float unitX = 1.0f / (float)countX;
	float unitY = 1.0f / (float)countY;

	u = (u - unitX * sx) * countX;
	v = (v - unitY * sy) * countY;

	std::array<glm::fvec3, 4> subPoints;
	std::array<glm::fvec3, 4> deCastiljeuPoints;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			deCastiljeuPoints[j] = points[pointIndex(sx, sy, j, i)]->getTransform().location;

		subPoints[i] = BezierCurve::deCastljeu(u, deCastiljeuPoints);
	}

	std::array<glm::fvec3, 3> derivative;

	for (int i = 0; i < 3; i++)
		derivative[i] = 3.0f * (subPoints[i + 1] - subPoints[i]);


	return BezierCurve::deCastljeu(v, derivative) * (float)countY;
}

bool SurfaceC0::wrappedU()
{
	return cylinder;
}

bool SurfaceC0::wrappedV()
{
	return false;
}

void SurfaceC0::acceptIntersection(std::weak_ptr<Intersection> intersection)
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

void SurfaceC0::removeIntersection(std::weak_ptr<Intersection>intersection)
{
	auto intLock = intersection.lock();

	std::erase_if(intersections, [intLock](const std::weak_ptr<Intersection>& i) {
		return i.lock().get() == intLock.get();
		});
	std::erase_if(intersectionTextures, [&intLock](const unsigned int& i) {
		return i == intLock->uvS1Tex || i == intLock->uvS2Tex;
		});
}

bool SurfaceC0::CanChildBeMoved() const
{
	return true;
}

bool SurfaceC0::canBeMoved() const
{
	return true;
}

void SurfaceC0::ChildMoved(ISceneElement& child)
{
	shouldReload = true;
}

void SurfaceC0::setRenderState(SurfaceRenderState state)
{
	// TODO implement soid surfaceC0 for milling paths
}
