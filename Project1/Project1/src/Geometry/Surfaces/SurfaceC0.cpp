#include "SurfaceC0.h"

Indexer SurfaceC0::indexer;

void SurfaceC0::updateMeshes()
{
	std::vector<int> inds;
	std::vector<float> vertices;

	for(int i =0; i<points.size() || i < positions.size(); i++) 
	{
		auto& pos = points.size() == 0 ? positions[i] : points[i]->getTransform().location;
		vertices.push_back(pos.x);
		vertices.push_back(pos.y);
		vertices.push_back(pos.z);
	}

	for (int i = 0; i < countY; i++)
		for (int j = 0; j < countX; j++)
		{
			int rowSize = cylinder ? countX * 3 : (countX * 3 + 1);
			int offset = 3 * i * rowSize + 3 * j;
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{
					if(3 * j + x == rowSize && cylinder)
						inds.push_back(3 * i * rowSize + y * rowSize);
					else
						inds.push_back((offset + y * rowSize + x) );
					
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
		for (int a = 0; a < countX; a++)
		{
			float angle1 = 2 * 3.14 * a / countX;
			float angle2 = 2 * 3.14 * (a+1) / countX;

			glm::fvec4 l1 = { sizeX * cosf(angle1) , sizeX * sinf(angle1) , h * distY, 0.0f };
			glm::fvec4 l2 = { sizeX * cosf(angle2) , sizeX * sinf(angle2) , h * distY, 0.0f };
			
			glm::fvec4 s1 = glm::fvec4(-sinf(angle1), cosf(angle1), 0.0f, 0.0f);
			glm::fvec4 s2 = glm::fvec4(-sinf(angle2), cosf(angle2), 0.0f, 0.0f);

			float cosA = glm::dot(glm::normalize(s1), glm::normalize(l2-l1));
			float mult = glm::length(l1-l2)/ 4.0f /cosA;

			positions.push_back(pos - middle + l1);
			positions.push_back(pos - middle + l1 + mult*glm::fvec4( -sinf(angle1), cosf(angle1), 0.0f, 0.0f ));
			positions.push_back(pos - middle + l2 - mult*glm::fvec4( -sinf(angle2), cosf(angle2), 0.0f, 0.0f ));

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
	name = json["name"];
	countX = json["size"]["x"];
	countY = json["size"]["y"];
	cylinder = json["parameterWrapped"]["u"];

	std::vector<nlohmann::json> patches = json["patches"];
	points = std::vector<std::shared_ptr<Point>>((cylinder ? countX * 3 : 1 + 3 * countX) * (1 + 3 * countY));

	for (int i = 0; i < countY; i++)
		for (int j = 0; j < countX; j++)
		{
			std::vector<int> path = patches[i * countX + j]["controlPoints"];
			int rowSize = cylinder ? countX * 3 : (countX * 3 + 1);
			int offset = 3 * i * rowSize + 3 * j;
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{
					if (3 * j + x == rowSize && cylinder)
						points[3 * i * rowSize + y * rowSize] = pointMap[path[y * 4+x]];
					else
						points[offset + y * rowSize + x] = pointMap[path[y * 4 + x]];

				}
		}
	updateMeshes();
}
SurfaceC0::SurfaceC0()
	:tessShader("Shaders/Surface/surfaceC0.vert", "Shaders/fragmentShader.frag"),
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
		for (auto& p : points)
			p->po.reset();
}

void SurfaceC0::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
}

void SurfaceC0::onMove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	if (std::find(points.begin(), points.end(), elem) != points.end())
		shouldReload = true;
}

bool SurfaceC0::CanBeDeleted(const Point& p) const
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
		updateMeshes();
	}
	ImGui::Checkbox("Show Chain", &showChain);
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
			std::vector<int> patchPoints;
			int rowSize = cylinder ? countX * 3 : (countX * 3 + 1);
			int offset = 3 * i * rowSize + 3 * j;
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{
					if (3 * j + x == rowSize && cylinder)
						patchPoints.push_back(pointIndexMap.find(points[3 * i * rowSize + y * rowSize]->getId())->second);
					else
						patchPoints.push_back(pointIndexMap.find(points[offset + y * rowSize + x]->getId())->second);
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
