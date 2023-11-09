#include "MillingPathCreator.h"
#include "../Rendering/TextureRenderer.h"
#include "SceneHeightMapRenderer.h"
#include "../Debuger/Debuger.h"
#include <numbers>
#include "../Geometry/Intersection.h"
#include "../TextureCPUAccess.h"

const glm::fvec4 MillingPathCreator::rect = { -25, 25,10,-40 };
const glm::fvec2 MillingPathCreator::zRange = { 0,-10 };
const glm::fvec3 MillingPathCreator::targetSize = { 15.0f, 5.0f, 15.0f };
const float MillingPathCreator::targetBaseHeight = 5.0f;
const float MillingPathCreator::epsilon = 0.001f;
const float MillingPathCreator::roughtPathsTranslation = 0.5f;


unsigned int MillingPathCreator::renderSceneHeightMap(Scene& scene)
{
	SceneHeightMapRenderer sceneRenderer(scene);
	TextureRenderer tr(textureSize, textureSize, 3, true);

	float rangeLen = zRange.y - zRange.x;

	// setup uniforms
	VariableManager vm;
	vm.AddVariable("projMtx", glm::ortho<float>(rect.x,rect.y,rect.z, rect.w, 0.001f, fabsf(rangeLen)*2));
	vm.AddVariable("viewMtx", glm::translate(glm::rotate<float>(glm::identity<glm::fmat4x4>(), std::numbers::pi, { 0,1,0 }), { 0,0,-rangeLen }));
	vm.AddVariable("modelMtx", glm::identity<glm::fmat4x4>());
	vm.AddVariable("color", glm::fvec4(0,1,1,1));
	vm.AddVariable("divisionV", 4);
	vm.AddVariable("divisionU", 4);
	vm.AddVariable("heightRange", zRange);

	// render
	tr.Clear({ 0,0,0,0 });
	tr.Render(sceneRenderer, vm);
	
	return tr.getTextureId();
}
std::vector<glm::fvec3> MillingPathCreator::sampleHeigthMap(unsigned int heightMap, float level, bool startTopLeft)
{
	float cadWidth = rect.y - rect.x;
	float cadHeight = fabsf(zRange.x - zRange.y);
	float materialLimitInCadSpace = targetSize.z * cadWidth / targetSize.x;
	float materialLimitInTextureSpace = materialLimitInCadSpace / cadHeight;
	float levelInTextureSpace = level * materialLimitInTextureSpace;
	float levelInTargetSpace = level * targetSize.y;

	TextureCPUAccess textureCPU(heightMap,textureSize,textureSize);

	std::vector<glm::fvec3> positions;
	positions.reserve(40 * 40);
	for (int i = 0; i < textureSize; i += 10)
	{
		for (int j = 0; j < textureSize; j += 10)
		{
			int k = (i / 10) % 2 == 0 ? j : textureSize - j - 1;
			glm::fvec3 positionTargetSpace = {
				i / (float)textureSize * targetSize.x - targetSize.x / 2.0f,
				k / (float)textureSize * targetSize.z - targetSize.z / 2.0f,
				0 };
			float heightTexSpace = textureCPU.getByIndex(i,k).x;
			float heightTargetSpace = targetSize.y * heightTexSpace / materialLimitInTextureSpace;
			positionTargetSpace.z = heightTexSpace < levelInTextureSpace ? levelInTargetSpace : heightTargetSpace;
			/*if (j == 0) {
				glm::fvec3 highPos = positionTargetSpace;
				highPos.z = (targetSize.y + 1)*10.0f;
				positions.push_back(highPos);
			}*/
			if (j == 0 || positionTargetSpace.y != positions.rbegin()->y)
				positions.push_back((positionTargetSpace + glm::fvec3(0.0f, 0.0f, targetBaseHeight)) * 10.0f);
		}
/*
		glm::fvec3 highPos = *positions.rbegin();
		highPos.z = (targetSize.y + 1) * 10.0f;
		positions.push_back(highPos);*/
	}
	if (!startTopLeft)  std::reverse(positions.begin(), positions.end());
	return positions;
}

MillingPath MillingPathCreator::CreateRoughtingPath(Scene& scene)
{
	unsigned int tex = renderSceneHeightMap(scene);
	Debuger::ShowTexture(tex);

	std::vector<glm::fvec3> positions = sampleHeigthMap(tex, 0.5f, true);
	std::vector<glm::fvec3> positions0 = sampleHeigthMap(tex, 0.0f, false);
	positions.insert(positions.end(), positions0.begin(), positions0.end());
	return MillingPath(positions, 12, false);
}

MillingPath MillingPathCreator::CreateBasePath(Scene& scene)
{
	std::shared_ptr<IUVSurface> base = findSurfaceByName(scene, "base");
	UVEnvelope envelope = createEnvelope(base);
	
	std::vector<glm::fvec3> positions;
	int lineCount = 20;
	bool reverse = false;
	for (int i = 0; i < lineCount; i++)
	{
		float v = 1 - i / (float)(lineCount-1);
		positions.push_back({ i==0 ? 0 : positions.rbegin()->x, v * 75, 0});

		std::vector<UVEnvelopeIterator> intersections = intersectEnvelopeWithV(envelope, v);
		if (intersections.empty() || i%2 == 1)
		{
			positions.push_back({ reverse ? 0 : 75, v*75, 0 });
		}
		else
		{
			glm::fvec2 minInt = { 1,v };
			int minI = 0;
			for (int j =0; j<intersections.size(); j++)
			{
				glm::fvec2 intUV = intersections[j].Current();
				if (intUV.x < minInt.x)
				{
					minI = j;
					minInt = intUV;
				}
			}
			positions.push_back({ minInt.x*75, minInt.y*75, 0 });
			
			UVEnvelopeIterator& it = intersections[minI];
			glm::fvec2 pos = it.Current();
			float nextV = 1 - (i+1) / (float)(lineCount - 1);
			while (!it.Finished() && pos.y > nextV)
			{
				pos = it.Next();
				positions.push_back({pos.x*75,pos.y*75,0});
			}
		}
		reverse = !reverse;
	}
	reverse = false;
	for (int i = 0; i < lineCount; i++)
	{
		float v = i / (float)(lineCount - 1);
		positions.push_back({ i == 0 ? 75 : positions.rbegin()->x, v * 75, 0 });

		std::vector<UVEnvelopeIterator> intersections = intersectEnvelopeWithV(envelope, v);
		if (intersections.empty() || i % 2 == 1)
		{
			positions.push_back({ reverse ? 75 : 0, v * 75, 0 });
		}
		else
		{
			glm::fvec2 maxInt = { 0,v };
			int maxI = 0;
			for (int j = 0; j < intersections.size(); j++)
			{
				glm::fvec2 intUV = intersections[j].Current();
				if (intUV.x > maxInt.x)
				{
					maxI = j;
					maxInt = intUV;
				}
			}
			positions.push_back({ maxInt.x * 75, maxInt.y * 75, 0 });

			UVEnvelopeIterator& it = intersections[maxI];
			glm::fvec2 pos = it.Current();
			float nextV = (i + 1) / (float)(lineCount - 1);
			while (!it.Finished() && pos.y < nextV)
			{
				pos = it.Next();
				positions.push_back({ pos.x * 75,pos.y * 75,0 });
			}
		}
		reverse = !reverse;
	}
	return MillingPath(positions,8,true);
}

MillingPath MillingPathCreator::CreateRoundingPath(Scene& scene)
{
	std::shared_ptr<IUVSurface> base = findSurfaceByName(scene, "base");
	UVEnvelope envelope = createEnvelope(base);
	UVEnvelopeIterator it(envelope);

	std::vector<glm::fvec3> positions;
	while (!it.Finished())
	{
		glm::fvec2 pos = it.Next();
		positions.push_back({ pos.x * 75.0f,pos.y * 75.0f,0 });
	}
	return MillingPath(positions,8,true);
}
std::shared_ptr<IUVSurface> MillingPathCreator::findSurfaceByName(Scene& scene, std::string name)
{
	for (auto& o : scene.objects)
	{
		auto surface = std::dynamic_pointer_cast<IUVSurface>(o.first);
		if (!surface)
			continue;
		if (o.first->getName() == name) 
			return surface;
	}
	return nullptr;
}

UVEnvelope MillingPathCreator::createEnvelope(std::shared_ptr<IUVSurface> base)
{
	std::vector<std::weak_ptr<Intersection>> baseIntersections = base->getIntersections();

	std::vector<UVCurve> curves;
	for (auto bi : baseIntersections)
	{
		std::shared_ptr<Intersection> baseIntersection = bi.lock();
		curves.push_back(baseIntersection->GetUVCurve(base));
	}

	UVEnvelope envelope;
	std::map<int, std::vector<std::pair<int, int>>> subCurves; // {UVCurveIndex, vector<UVVertexIndex, curvePointIndex>}
	for (int i = 0; i < curves.size(); i++)
		subCurves.insert({ i,{} });

	for(int i = 0; i < curves.size(); i++)
		for (int j = 0; j < i; j++)
		{
			for (auto p : findIntersections(curves[i], curves[j]))
			{
				envelope.vertices.emplace_back(curves[i].positions[p.first]);
				int ind = envelope.vertices.size() - 1;
				subCurves[i].push_back({ ind, p.first });
				subCurves[j].push_back({ ind, p.second });
			}
		}

	for (auto& c : subCurves)
	{
		if (c.second.size() != 2)
			continue;
		envelope.curves.emplace_back(curves[c.first], 
			&envelope.vertices[c.second[0].first], &envelope.vertices[c.second[1].first], 
			c.second[0].second, c.second[1].second);
	}
	for (int i = 0; i < envelope.curves.size(); i++)
	{
		SubCurve* added = &envelope.curves[i];
		added->v1->curves.push_back(added);
		added->v2->curves.push_back(added);
	}
	return envelope;
}

std::vector<std::pair<int, int>> MillingPathCreator::findIntersections(UVCurve c1, UVCurve c2)
{
	std::vector<std::pair<int, int>> intersections;
	for (int i = 1; i < c1.positions.size(); i++)
		for (int j = 1; j < c2.positions.size(); j++)
		{
			glm::fvec2 c1Pos = c1.positions[i];
			glm::fvec2 c2Pos = c2.positions[j];
			glm::fvec2 c1Last = c1.positions[i-1];
			glm::fvec2 c2Last = c2.positions[j-1];
			if (segmentIntersect(c1Last, c1Pos, c2Last, c2Pos))
			{
				Debuger::ShowUVPoint(c1Last);
				intersections.emplace_back(i - 1, j - 1);
			}
		}
	return intersections;
}

bool MillingPathCreator::segmentIntersect(glm::fvec2 a, glm::fvec2 b, glm::fvec2 c, glm::fvec2 d)  // odcinki ab i cd
{
	glm::fvec2 cd = d - c;
	glm::fvec2 cb = b - c;
	glm::fvec2 ca = a - c;
	glm::fvec2 ab = b - a;
	glm::fvec2 ac = c - a;
	glm::fvec2 ad = d - a;

	float cdcb = cd.x * cb.y - cd.y * cb.x;
	float cdca = cd.x * ca.y - cd.y * ca.x;
	float abac = ab.x * ac.y - ab.y * ac.x;
	float abad = ab.x * ad.y - ab.y * ad.x;

	return glm::sign(cdcb) != glm::sign(cdca) && glm::sign(abac) != glm::sign(abad);
}

std::vector<UVEnvelopeIterator> MillingPathCreator::intersectEnvelopeWithV(UVEnvelope& envelope, float v)
{
	std::vector<UVEnvelopeIterator> positions;
	UVEnvelopeIterator it(envelope);
	glm::fvec2 last = it.Current();
	while (!it.Finished())
	{
		glm::fvec2 current = it.Next();
		if ((current.y > v && last.y < v) || (current.y < v && last.y > v))
			positions.push_back(UVEnvelopeIterator(it)); // TODO dodaæ kierunek
		last = current;
	}
	return positions;
}