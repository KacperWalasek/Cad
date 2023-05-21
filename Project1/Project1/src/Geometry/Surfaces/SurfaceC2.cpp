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
	float distX = sizeX / (countX + 1);
	float distY = sizeY / (countY + 1);
	for (int i = 0; i < countY + 3; i++)
		for (int j = 0; j < countX + 3; j++)
			positions.push_back(pos + glm::fvec4(i * distX, 0.0f, j * distY, 0.0f));
}

void SurfaceC2::CreatePointsCylinder()
{
	float distY = sizeY / (countY * 1);
	for(int h = 0; h < countY + 3; h++)
		for (int a = 0; a < countX; a++)
		{
			float angle1 = 2 * 3.14 * a / countX;
			positions.push_back(pos + glm::fvec4( sizeX * cosf(angle1) , sizeX * sinf(angle1) , h * distY, 0.0f ));

		}

	//float distY = sizeY / (countY * 3);
	//for (int h = 0; h < 1 + countY * 3; h++)
	//	for (int a = 0; a < countX; a++)
	//	{
	//		float angle1 = 2 * 3.14 * a / countX;
	//		float angle2 = 2 * 3.14 * (a + 1) / countX;

	//		glm::fvec4 l1 = { sizeX * cosf(angle1) , sizeX * sinf(angle1) , h * distY, 0.0f };
	//		glm::fvec4 l2 = { sizeX * cosf(angle2) , sizeX * sinf(angle2) , h * distY, 0.0f };

	//		glm::fvec4 s1 = glm::fvec4(-sinf(angle1), cosf(angle1), 0.0f, 0.0f);
	//		glm::fvec4 s2 = glm::fvec4(-sinf(angle2), cosf(angle2), 0.0f, 0.0f);

	//		float cosA = glm::dot(glm::normalize(s1), glm::normalize(l2 - l1));
	//		float mult = glm::length(l1 - l2) / 4.0f / cosA;

	//		positions.push_back(pos + l1);
	//		positions.push_back(pos + l1 + mult * glm::fvec4(-sinf(angle1), cosf(angle1), 0.0f, 0.0f));
	//		positions.push_back(pos + l2 - mult * glm::fvec4(-sinf(angle2), cosf(angle2), 0.0f, 0.0f));

	//	}
}

SurfaceC2::SurfaceC2(glm::fvec4 pos, int countX, int countY, float sizeX, float sizeY, bool cylinder)
	: name("SurfaceC0-" + std::to_string(indexer.getNewIndex())),
	countX(countX),
	countY(countY),
	sizeX(sizeX),
	sizeY(sizeY),
	cylinder(cylinder),
	pos(pos),
	tessShader("Shaders/Surface/surfaceC0.vert", "Shaders/fragmentShader.frag"),
	chainShader("Shaders/Surface/surfaceC0.vert", "Shaders/fragmentShader.frag"),
	shouldReload(false),
	showChain(false)
{
	division[0] = 4;
	division[1] = 4;

	tessShader.Init();
	tessShader.loadShaderFile("Shaders/Surface/surfaceC0.tesc", GL_TESS_CONTROL_SHADER);
	tessShader.loadShaderFile("Shaders/Surface/surfaceC2.tese", GL_TESS_EVALUATION_SHADER);
	chainShader.Init();
	chainShader.loadShaderFile("Shaders/Surface/surfaceChain.tesc", GL_TESS_CONTROL_SHADER);
	chainShader.loadShaderFile("Shaders/Surface/surfaceChain.tese", GL_TESS_EVALUATION_SHADER);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);

	Recalculate();
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
