#include "MillingPathCreator.h"
#include "../Rendering/TextureRenderer.h"
#include "SceneHeightMapRenderer.h"
#include "../Debuger/Debuger.h"
#include <numbers>
#include "../Geometry/Intersection.h"
#include "../TextureCPUAccess.h"
#include "../Geometry/Surfaces/SurfaceShift.h"

const glm::fvec4 MillingPathCreator::rect = { -25, 25,10,-40 };
const glm::fvec2 MillingPathCreator::zRange = { 0,-10 };
const glm::fvec3 MillingPathCreator::targetSize = { 15.0f, 5.0f, 15.0f };
const float MillingPathCreator::targetBaseHeight = 5.0f;
const float MillingPathCreator::epsilon = 0.001f;
const float MillingPathCreator::roughtPathsTranslation = 0.5f;


void MillingPathCreator::intersect(Scene& scene, std::shared_ptr<IUVSurface> s1, std::shared_ptr<IUVSurface> s2, glm::fvec3 cursorPos)
{
	auto intersection = std::make_shared<Intersection>(s1, s2, true, cursorPos, 0.5f);
	if (!intersection->valid)
	{
		std::cout << "invalid intersection" << std::endl;
		return;
	}
	scene.Add(intersection);
	s1->acceptIntersection(intersection);
	s2->acceptIntersection(intersection);
}

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

glm::fvec3 MillingPathCreator::worldToTargetSpace(glm::fvec3 pos)
{
	float xTexSpace = (pos.x - rect.x) / (rect.y - rect.x);
	float yTexSpace = (pos.y - rect.w) / (rect.z - rect.w);
	float zTexSpace = pos.z / zRange.y;
	return  {
		xTexSpace * targetSize.x - targetSize.x / 2.0f,
		yTexSpace * targetSize.z - targetSize.z / 2.0f,
		zTexSpace * targetSize.y };
	
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
	const float r = 1.2f;
	std::shared_ptr<IUVSurface> base = findSurfaceByName(scene, "base");
	std::shared_ptr<IUVSurface> handle = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "handle"), r, true);
	std::shared_ptr<IUVSurface> body = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "body"), r, false);

	intersect(scene, handle, base, { 0, 2, -r });
	intersect(scene, body, base, { 0, -10, -r });

	UVEnvelope envelope = createEnvelope(base);
	
	std::vector<glm::fvec2> positionsUV;
	int lineCount = 20;
	bool reverse = false;
	for (int i = 0; i < lineCount; i++)
	{
		float v = 1 - i / (float)(lineCount - 1);
		if (i == 0)
		{
			positionsUV.push_back({ 1, v });
			positionsUV.push_back({ 0, v });
			continue;
		}
		positionsUV.push_back({positionsUV.rbegin()->x, v });

		std::vector<UVEnvelopeIterator> intersections = intersectEnvelopeWithLine(envelope, v, false);
		if (intersections.empty() || i % 2 == 0  )
		{
			positionsUV.push_back({ reverse ? 0 : 1, v });
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
			positionsUV.push_back({ minInt.x, minInt.y });
			
			UVEnvelopeIterator& it = intersections[minI];
			glm::fvec2 pos = it.Current();
			float nextV = 1 - (i+1) / (float)(lineCount - 1);
			while (!it.Finished() && pos.y > nextV)
			{
				glm::fvec2 nextPos = it.Next();
				if (pos.y < nextPos.y || nextPos.y <= nextV)
					break;
				pos = nextPos;
				positionsUV.push_back({ pos.x, pos.y });
			}
		}
		reverse = !reverse;
	}
	reverse = false;
	for (int i = 0; i < lineCount; i++)
	{
		float v = i / (float)(lineCount - 1);
		if (i == 0)
		{
			positionsUV.push_back({ 0, v });
			positionsUV.push_back({ 1, v });
			continue;
		}
		positionsUV.push_back({positionsUV.rbegin()->x, v });

		std::vector<UVEnvelopeIterator> intersections = intersectEnvelopeWithLine(envelope, v, false);
		if (intersections.empty() || i % 2 == 0)
		{
			positionsUV.push_back({ reverse ? 1 : 0, v });
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
			positionsUV.push_back({ maxInt.x, maxInt.y });

			UVEnvelopeIterator& it = intersections[maxI];
			glm::fvec2 pos = it.Current();
			float nextV = (i + 1) / (float)(lineCount - 1);
			while (!it.Finished() && pos.y < nextV)
			{
				glm::fvec2 nextPos = it.Next();
				if (pos.y > nextPos.y || nextPos.y >= nextV)
					break;
				pos = nextPos;
				positionsUV.push_back({ pos.x,pos.y });
			}
		}
		reverse = !reverse;
	}
	return MillingPath(uvPathsToWorldPathsFlat(base,positionsUV), 8, true);
}

MillingPath MillingPathCreator::CreateRoundingPath(Scene& scene)
{
	const float r = 1.2f;
	std::shared_ptr<IUVSurface> base = findSurfaceByName(scene, "base");
	std::shared_ptr<IUVSurface> handle = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "handle"), r, true);
	std::shared_ptr<IUVSurface> body = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "body"), r, false);

	intersect(scene, handle, base, { 0, 2, -r });
	intersect(scene, body, base, { 0, -10, -r });
	
	UVEnvelope envelope = createEnvelope(base);
	//envelope.curves[1].reverseDirection = true;
	UVEnvelopeIterator it(envelope);

	std::vector<glm::fvec2> uvPositions;
	while (!it.Finished())
	{
		glm::fvec2 pos = it.Next();
		uvPositions.push_back({ pos.x, pos.y });
	}
	return MillingPath(uvPathsToWorldPathsFlat(base, uvPositions), 8, true);
}

MillingPath MillingPathCreator::CreateDetailPath(Scene& scene)
{
	const float r = 1.0f;
	std::shared_ptr<IUVSurface> base = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "base"), r, false);
	std::shared_ptr<IUVSurface> handle = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "handle"), r, true);
	std::shared_ptr<IUVSurface> body = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "body"),  r, false);
	std::shared_ptr<IUVSurface> button = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "button"), r, false);
	if (!base)
		throw std::logic_error("no base surface found");
	if (!handle)
		throw std::logic_error("no handle surface found");
	if (!body)
		throw std::logic_error("no body surface found");
	if (!button)
		throw std::logic_error("no button surface found");

	Debuger::ShowUVSurface(body);
	Debuger::ShowUVSurface(handle);
	Debuger::ShowUVSurface(body);
	Debuger::ShowUVSurface(button);


	intersect(scene, body, handle, { 7, -9, -r });
	intersect(scene, body, handle, { -4, -9, -r });
	//intersect(scene, handle, base, { 0, -2, -r });
	intersect(scene, body, base, { 0, -10, -r });
	intersect(scene, body, button, { 0, -12, -4 });

	//UVEnvelope envelopeHole = createEnvelope(base);

	intersect(scene, handle, base, { 0, 2, -r });
	
	UVEnvelope envelopeBody = createEnvelope(body);
	envelopeBody.curves[1].reverseDirection = true;
	envelopeBody.curves[2].reverseDirection = true;
	
	//UVEnvelope envelopeHandle = createEnvelope(handle);
	//envelopeHandle.curves[0].reverseDirection = true;

	//UVEnvelope envelopeButton = createEnvelope(button);
	
	std::vector<glm::fvec3> positionsBody = millUVSurface(body, envelopeBody, Direction::UpRight);
	//std::vector<glm::fvec3> positionsHandle = millUVSurface(handle, envelopeHandle, Direction::UpRight, true);
	//std::vector<glm::fvec3> positionsHole = millUVSurface(base, envelopeHole, Direction::UpRight);
	//std::vector<glm::fvec3> positionsButton = millUVSurface(button, envelopeButton, Direction::UpRight, true);
	
	std::vector<glm::fvec3> positions;
	positions.insert(positions.end(), positionsBody.begin(), positionsBody.end());
	//positions.insert(positions.end(), positionsHandle.begin(), positionsHandle.end());
	//positions.insert(positions.end(), positionsHole.begin(), positionsHole.end());
	//positions.insert(positions.end(), positionsButton.begin(), positionsButton.end());

	return MillingPath(positions, 8, true);
}

std::vector<glm::fvec3> MillingPathCreator::millUVSurface(std::shared_ptr<IUVSurface> s, UVEnvelope& envelope, Direction direction, bool borderBottom, bool side)
{
	std::vector<glm::fvec2> uvPositions;
	bool reverse = false;
	int lineCount = 40;
	for (int i = 0; i < lineCount; i++)
	{
		float u = i / (float)(lineCount - 1);
		auto intersections = intersectEnvelopeWithLine(envelope, u, true);
		if (intersections.size()==0 || (intersections.size() == 1 && !borderBottom))
			continue;
		int minInd = -1;
		int maxInd = -1;
		glm::fvec2 minUV = { u, borderBottom ? 0 : 1 };
		glm::fvec2 maxUV = { u, 0 };
		for (int j = 0; j < intersections.size(); j++)
		{
			glm::fvec2 current = intersections[j].Current();
			if (current.y < minUV.y)
			{
				minUV = current;
				minInd = j;
			}
			if (current.y > maxUV.y)
			{
				maxUV = current;
				maxInd = j;
			}
		}
		if (reverse)
		{
			uvPositions.push_back(maxUV);
			uvPositions.push_back(minUV);
			if (intersections.size() > 1 && !borderBottom)
			{
				UVEnvelopeIterator& it = intersections[minInd];
				if (!it.GoesInPositiveU())
					it.Reverse();
				glm::fvec2 pos = it.Current();
				float nextU = (i + 1) / (float)(lineCount - 1);
				while (!it.Finished() && pos.x < nextU)
				{
					uvPositions.push_back(pos);
					pos = it.Next();
				}
			}
		}
		else
		{
			uvPositions.push_back(minUV);
			uvPositions.push_back(maxUV);
			if (intersections.size() > 1)
			{
				UVEnvelopeIterator& it = intersections[maxInd];
				if (!it.GoesInPositiveU())
					it.Reverse();
				glm::fvec2 pos = it.Current();
				float nextU = (i + 1) / (float)(lineCount - 1);
				while (!it.Finished() && pos.x < nextU)
				{
					uvPositions.push_back(pos);
					pos = it.Next();
				}
			}
		}
		reverse = !reverse;
	}
/*
	UVEnvelopeIterator it(envelope);

	while (!it.Finished())
	{
		glm::fvec2 pos = it.Next();
		positions.push_back(pos);
	}*/
	std::vector<glm::fvec3> positions;
	for (auto p : uvPositions)
	{
		positions.push_back({ p.x*150.0f - 75.0f,p.y*150.0f - 75.0f,0.0f });
	}
	return positions;// uvPathsToWorldPathsFlat(s, uvPositions);// , side);
}

std::vector<glm::fvec3> MillingPathCreator::uvPathsToWorldPaths(std::shared_ptr<IUVSurface> s, std::vector<glm::fvec2> uvPositions, bool side)
{
	//float sideMult = side ? -1 : 1;
	std::vector<glm::fvec3> worldPositions;
	for (int i = 0; i < uvPositions.size() - 1; i++)
	{
		glm::fvec2 uvPrev = uvPositions[i];
		glm::fvec2 uvNext = uvPositions[i+1];
		/*glm::fvec3 norm = glm::normalize(glm::cross(
			s->dfdv(uvPrev.x, uvPrev.y),
			s->dfdu(uvPrev.x, uvPrev.y)));*/
		worldPositions.push_back(10.0f * worldToTargetSpace(s->f(uvPrev.x, uvPrev.y)));// +sideMult * glm::normalize(worldToTargetSpace(norm)) * 8.0f);
		const float dy = 0.1f;
		float t = 0;
		glm::fvec2 uv = uvPrev;
		while (t < 1)
		{
			glm::fvec3 dfdu = s->dfdu(uv.x, uv.y);
			glm::fvec3 dfdv = s->dfdv(uv.x, uv.y);
			glm::fvec3 dfdt = dfdu * (uvNext.x - uvPrev.x) + dfdv * (uvNext.y - uvPrev.y);
			t += dy / dfdt.length();
			t = glm::clamp(t, 0.0f, 1.0f);
			uv = uvPrev * (1 - t) + uvNext * t;
			/*
			glm::fvec3 norm = glm::normalize(glm::cross(
				s->dfdu(uv.x, uv.y),
				s->dfdv(uv.x, uv.y)
				));
			if (norm.z > 0)
			{
				//std::cout << std::dynamic_pointer_cast<ISceneElement>(s)->getName() << ": (" << norm.x << "," << norm.y << "," << norm.z << ")" << std::endl;
				norm.z = 0;
				norm = glm::normalize(norm);
			}
			*/
			/*glm::fvec3 dirPoint = 10.0f * worldToTargetSpace(s->f(uv.x, uv.y)/* + norm*//*);*/
			glm::fvec3 surfPoint = 10.0f * worldToTargetSpace(s->f(uv.x, uv.y));
			//glm::fvec3 normVec = glm::normalize(dirPoint - surfPoint) * 4.0f;
			worldPositions.push_back(surfPoint/* + normVec*/);
			//std::cout << std::dynamic_pointer_cast<ISceneElement>(s)->getName() << ": (" << norm.x << "," << norm.y << "," << norm.z << ")" << std::endl;
		}
	}
	return worldPositions;
}

std::vector<glm::fvec3> MillingPathCreator::uvPathsToWorldPathsFlat(std::shared_ptr<IUVSurface> s, std::vector<glm::fvec2> uvPositions)
{
	std::vector<glm::fvec3> positions;
	for (auto p : uvPositions)
	{
		positions.push_back(10.0f * worldToTargetSpace(s->f(p.x, p.y)));
	}
	return positions;
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
		if (c.second.size() > 1)
		{
			std::sort(c.second.begin(), c.second.end(), [](const auto& a, const auto& b) -> bool
				{
					return a.second < b.second;
				});
			for (int i = 1; i < c.second.size()-1; i+=2)
			{
				envelope.curves.emplace_back(curves[c.first],
					&envelope.vertices[c.second[i].first], &envelope.vertices[c.second[i+1].first],
					c.second[i].second, c.second[i+1].second);
			}
			envelope.curves.emplace_back(curves[c.first],
				&envelope.vertices[c.second[0].first], &envelope.vertices[c.second.rbegin()->first],
				c.second[0].second, c.second.rbegin()->second);
		}
		else
		{
			envelope.curves.emplace_back(curves[c.first],
				c.second.size() > 0 ? &envelope.vertices[c.second[0].first] : nullptr, nullptr,
				c.second.size() > 0 ? c.second[0].second : 0, curves[c.first].positions.size() - 1);
		}
	}

	for (int i = 0; i < envelope.curves.size(); i++)
	{
		SubCurve* added = &envelope.curves[i];
		if (added->v1)
			added->v1->curves.push_back(added);
		if (added->v2)
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
			if (fabsf(c1Pos.x - c1Last.x) > 0.9 || fabsf(c2Pos.x - c2Last.x) > 0.9 ||
				fabsf(c1Pos.y - c1Last.y) > 0.9 || fabsf(c2Pos.y - c2Last.y) > 0.9)
				continue; //przejscie na druga strone przestrzeni uv
			if (segmentIntersect(c1Last, c1Pos, c2Last, c2Pos))
			{
				//Debuger::ShowUVPoint(c1Last);
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

std::vector<UVEnvelopeIterator> MillingPathCreator::intersectEnvelopeWithLine(UVEnvelope& envelope, float t, bool u)
{
	std::vector<UVEnvelopeIterator> positions;
	UVEnvelopeIterator it(envelope);
	glm::fvec2 last = it.Current();
	while (!it.Finished())
	{
		glm::fvec2 current = it.Next();
		if (fabsf(current.x - last.x) < 0.5f && fabsf(current.y - last.y) < 0.5f)
		{
			if (u && ((current.x >= t && last.x < t) || (current.x <= t && last.x > t)) ||
				!u && ((current.y >= t && last.y < t) || (current.y <= t && last.y > t)))
				positions.push_back(UVEnvelopeIterator(it)); // TODO dodaæ kierunek
		}
		last = current;
	}
	return positions;
}