#include "SurfaceC2.h"

Indexer SurfaceC2::indexer;

void SurfaceC2::updateMeshes()
{
	std::vector<int> inds;
	std::vector<float> vertices;

	for (int i = 0; i < points.size() || i < positions.size(); i++)
	{
		auto& pos = points.size() == 0 ? positions[i] : points[i]->getTransform().location;
		vertices.push_back(pos.x);
		vertices.push_back(pos.y);
		vertices.push_back(pos.z);
	}

	for(int i = 0; i < countY; i++)
		for (int j = 0; j < countX; j++)
		{
			int rowSize = cylinder ? countX : countX + 3;
			int offset = i * rowSize + j;
			for(int x = 0; x < 4; x++)
				for (int y = 0; y < 4; y++)
				{
					if(cylinder && j+x>=countX)
						inds.push_back(offset + y * rowSize + x - countX);
					else
						inds.push_back(offset + y * rowSize + x);
				}
		}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
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
		for (int a = 0; a < countX; a++)
		{
			float angle1 = 2 * 3.14 * a / countX;
			positions.push_back(pos - middle + glm::fvec4( R * cosf(angle1) , R * sinf(angle1) , h * distY, 0.0f ));

		}
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
	name = json["name"];
	countX = json["size"]["x"];
	countY = json["size"]["y"];
	cylinder = json["parameterWrapped"]["u"];

	std::vector<nlohmann::json> patches = json["patches"];
	points = std::vector<std::shared_ptr<Point>>((cylinder ? countX : 3 + countX) * (3 + countY));
	
	division[0] = patches[0]["samples"]["x"];
	division[1] = patches[0]["samples"]["y"];

	float rowSize = cylinder ? countX : 3 + countX;
	for (int i = 0; i < countY; i++)
		for (int j = 0; j < countX-3; j++)
		{
			std::vector<int> patch = patches[i * countX + j]["controlPoints"];
			if (i == 0 && j == 0)
			{
				for (int x = 0; x < 3; x++)
					for (int y = 0; y < 3; y++)
						points[x * rowSize + y] = pointMap[patch[y * 4 + x]];
			}
			if (j == 0)
				for(int x = 0; x<3; x++)
					points[(3 + i)* rowSize + x] = pointMap[patch[3 + x * 4]];
			if (i == 0)
				for (int y = 0; y < 3; y++)
					points[y * rowSize + (3 + j)] = pointMap[patch[12 + y]];

			points[(3 + i) * rowSize + (3 + j)] = pointMap[patch[15]];
		}

	updateMeshes();
}
SurfaceC2::SurfaceC2()
	: tessShader("Shaders/Surface/surfaceC0.vert", "Shaders/fragmentShader.frag"),
	chainShader("Shaders/Surface/surfaceC0.vert", "Shaders/fragmentShader.frag"),
	shouldReload(false),
	showChain(false)
{

	tessShader.Init();
	tessShader.loadShaderFile("Shaders/Surface/surfaceC0.tesc", GL_TESS_CONTROL_SHADER);
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
			p->po.reset();
}

void SurfaceC2::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
}

void SurfaceC2::onMove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	if (std::find(points.begin(), points.end(), elem) != points.end())
		shouldReload = true;
}

bool SurfaceC2::CanBeDeleted(const Point& p) const
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
		updateMeshes();
	}
	ImGui::Checkbox("Show Chain", &showChain);
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
			std::vector<int> patchPoints;
			int rowSize = cylinder ? countX : countX + 3;
			int offset = i * rowSize + j;

			for (int x = 0; x < 4; x++)
				for (int y = 0; y < 4; y++)
				{
					if (cylinder && j + x >= countX)
						patchPoints.push_back(pointIndexMap.find(points[offset + y * rowSize + x - countX]->getId())->second);
					else
						patchPoints.push_back(pointIndexMap.find(points[offset + y * rowSize + x]->getId())->second);
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
