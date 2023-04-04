#include "CurveC0.h"
#include <imgui/imgui_stdlib.h>
#include "Scene.h"
#include <glm/gtc/type_ptr.hpp>

CurveC0::CurveC0(Camera& camera)
	: CurveC0(camera, {})
{
}

CurveC0::CurveC0(Camera& camera, std::vector<std::shared_ptr<Point>> points)
	: camera(camera), points(points), name("Curve"), addSelected(false), 
	removeSelected(false), showChain(true),
	shader("Shaders/test.vert", "Shaders/fragmentShader.frag")
{
	shader.Init();
	shader.loadShaderFile("Shaders/test.tesc", GL_TESS_CONTROL_SHADER);
	shader.loadShaderFile("Shaders/test.tese", GL_TESS_EVALUATION_SHADER);

	UpdateMeshes();
}

std::string CurveC0::getName() const
{
	return name;
}

void CurveC0::Render()
{
	UpdateMeshes();
	if (showChain)
		chainMesh.Render();
	shader.use();
	unsigned int colorLoc = glGetUniformLocation(shader.ID, "color");
	glUniform4f(colorLoc, 0.0f, 1.0f, 1.0f, 1.0f);

	unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
	glm::fmat4x4 centerMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(centerMatrix));
	
	glBindVertexArray(curveMesh.VAO);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawElements(GL_PATCHES, curveMesh.indices.size(), GL_UNSIGNED_INT, 0);
}

void CurveC0::RenderGui()
{
	ImGui::Begin("C0 Curve");
	ImGui::InputText("Name", &name);

	if (ImGui::Checkbox("Add on select", &addSelected))
		if (addSelected)
			removeSelected = false;
	if (ImGui::Checkbox("Remove on select", &removeSelected))
		if (removeSelected)
			addSelected = false;

	ImGui::Checkbox("Show chain", &showChain);
	ImGui::End();
}

void CurveC0::UpdateMeshes()
{
	beziers.clear();
	chainMesh = Mesh();
	curveMesh = Mesh();
	if (points.size() > 1)
	{
		std::vector<float> vert;
		std::vector<int> ind;
		int fullBeziersCount = fmax(0, floor((points.size() - 1.0f) / 3.0f));
		int fullPointCount = fullBeziersCount == 0 ? 1 : fullBeziersCount * 3 + 1;


		std::vector<glm::fvec4> last = { points[fullPointCount - 1]->getTransform().location };
		for (int i = 0; i < points.size(); i++)
		{
			glm::fvec4 p = points[i]->getTransform().location;
			chainMesh.vertices.push_back(p.x);
			chainMesh.vertices.push_back(p.y);
			chainMesh.vertices.push_back(p.z);

			if (i < fullPointCount)
			{
				curveMesh.vertices.push_back(p.x);
				curveMesh.vertices.push_back(p.y);
				curveMesh.vertices.push_back(p.z);
			}
			else
				last.push_back(p);
		}

		for (int i = 0; i < fullBeziersCount; i++)
		{
			curveMesh.indices.push_back(i * 3);
			curveMesh.indices.push_back(i * 3 + 1);
			curveMesh.indices.push_back(i * 3 + 2);
			curveMesh.indices.push_back(i * 3 + 3);
			for (int j = 0; j < 3; j++)
			{
				chainMesh.indices.push_back(i * 3 + j);
				chainMesh.indices.push_back(i * 3 + j + 1);
			}
		}
		for (int j = 0; j < (int)(points.size() - fullPointCount); j++)
		{
			chainMesh.indices.push_back(fullPointCount - 1 + j);
			chainMesh.indices.push_back(fullPointCount - 1 + j + 1);
		}
		if (last.size() > 1)
		{
			while (last.size() != 4)
			{
				float m = last.size();
				last.push_back(last[m - 1]);
				for (int i = m - 1; i > 0; i--)
					last[i] = (i / m) * last[i - 1] + ((m - i) / m) * last[i];

			}
			for (int i = 0; i < 3; i++)
			{
				curveMesh.indices.push_back(fullPointCount - 1 + i);
				curveMesh.vertices.push_back(last[i + 1].x);
				curveMesh.vertices.push_back(last[i + 1].y);
				curveMesh.vertices.push_back(last[i + 1].z);
			}
			curveMesh.indices.push_back(fullPointCount - 1 + 3);
		}
	}
	chainMesh.Update();
	curveMesh.Update();
}


void CurveC0::onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem)
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
			points.push_back(p);
	}
}

void CurveC0::onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	std::shared_ptr<Point> p = std::dynamic_pointer_cast<Point>(elem);

	auto it = std::find(points.begin(), points.end(), elem);
	if (it != points.end())
		points.erase(it);

}

void CurveC0::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
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
					points.push_back(p);
			}
		}
		else
			if (removeSelected)
			{
				points.erase(pointIt);
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
