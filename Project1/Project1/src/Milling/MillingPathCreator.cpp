#include "MillingPathCreator.h"
#include "../Rendering/TextureRenderer.h"
#include "SceneHeightMapRenderer.h"
#include "../Debuger/Debuger.h"
#include <numbers>
#include "../Geometry/Intersection.h"
#include "../TextureCPUAccess.h"
#include "../Geometry/Surfaces/SurfaceShift.h"

const glm::fvec4 MillingPathCreator::rect = { -22.5, 22.5,7.5,-37.5 }; // both must sum to 45
const glm::fvec2 MillingPathCreator::zRange = { 0,-15 }; // must sum to 15
const glm::fvec3 MillingPathCreator::targetSize = { 15.0f, 5.0f, 15.0f };
const float MillingPathCreator::targetBaseHeight = 5.0f;
const float MillingPathCreator::epsilon = 0.001f;
const float MillingPathCreator::roughtPathsTranslation = 0.5f;
const float MillingPathCreator::uDistMult = 1.0f;
const float MillingPathCreator::vDistMult = 0.001f;


void MillingPathCreator::intersect(Scene& scene, std::shared_ptr<IUVSurface> s1, std::shared_ptr<IUVSurface> s2, glm::fvec3 cursorPos)
{

	auto intersection = std::make_shared<Intersection>(s1, s2, true, cursorPos, 0.05f);
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

float MillingPathCreator::targetToWorldSpace(float dist)
{
	return dist*3.0f;
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
	const float r = targetToWorldSpace(1.2f);
	std::shared_ptr<IUVSurface> base = findSurfaceByName(scene, "base");
	std::shared_ptr<IUVSurface> handle = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "handle"), r, true);
	std::shared_ptr<IUVSurface> body = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "body"), r, false);

	intersect(scene, base, handle, { 0, 2, -r });
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
	const float r = targetToWorldSpace(0.6f);
	std::shared_ptr<IUVSurface> base = findSurfaceByName(scene, "base");
	std::shared_ptr<IUVSurface> handle = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "handle"), r, true);
	std::shared_ptr<IUVSurface> body = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "body"), r, false);

	intersect(scene, handle, base, { 0, 2, -r });
	intersect(scene, body, base, { 0, -10, -r });
	
	UVEnvelope envelope = createEnvelope(base);
	UVEnvelopeIterator it(envelope);
	return MillingPath(getRoundingPositions(it,base), 8, true);
}

MillingPath MillingPathCreator::CreateDetailPath(Scene& scene)
{
	const float r = targetToWorldSpace(0.4f);
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

	//Debuger::ShowUVSurface(body);
	//Debuger::ShowUVSurface(handle);
	//Debuger::ShowUVSurface(body);
	//Debuger::ShowUVSurface(button);

	intersect(scene, body, handle, { 7, -9, -r });
	intersect(scene, body, handle, { -4, -9, -r });
	intersect(scene, handle, base, { 0, -1.3 - r, 0 });
	intersect(scene, body, base, { 0, -10, -r });
	intersect(scene, body, button, { 0, -12, -4 });

	UVEnvelope envelopeHole = createEnvelope(base);
	envelopeHole.curves[1].reverseDirection = true;

	intersect(scene, handle, base, { 0, 2, -r });
	
	UVEnvelope envelopeBody = createEnvelope(body);
	envelopeBody.curves[0].reverseDirection = true;
	envelopeBody.curves[1].reverseDirection = true;
	envelopeBody.curves[3].reverseDirection = true;
	
	UVEnvelope envelopeHandle = createEnvelope(handle);
	envelopeHandle.curves[0].reverseDirection = true;
	envelopeHandle.curves[1].reverseDirection = true;

	UVEnvelope envelopeButton = createEnvelope(button);
	
	auto positionsBody = millUVSurface(body, envelopeBody);
	auto positionsHandle = millUVSurface(handle, envelopeHandle);
	auto positionsHole = millUVSurface(base, envelopeHole);
	auto positionsButton = millUVSurface(button, envelopeButton, true);

	auto positionsBodyToHandle = safeMove(positionsBody.back(), positionsHandle.front());
	auto positionsHandleToHole = safeMove(positionsHandle.back(), positionsHole.front());
	auto positionsHoleToButton = safeMove(positionsHole.back(), positionsButton.front());
	
	UVEnvelopeIterator it(envelopeButton);
	//std::vector<glm::fvec3> positions = getRoundingPositions(it, body, true);
	/*std::vector<glm::fvec3> positions;// = millUVSurface(base, envelopeHole);
	*/
	std::vector<glm::fvec3> positions = mergePaths({
			positionsBody,
			positionsBodyToHandle,
			positionsHandle,
			positionsHandleToHole,
			positionsHole,
			positionsHoleToButton,
			positionsButton
		});

	return MillingPath(positions, 8, true);
}

std::vector<glm::fvec3> MillingPathCreator::millUVSurface(std::shared_ptr<IUVSurface> s, UVEnvelope& envelope, bool borderBottom, bool side, bool asUV)
{
	std::vector<glm::fvec2> uvPositions;
	bool reverse = false;
	float u = 0;
	while (u < 1.0f)
	{
		float nextU;
		auto intersections = intersectEnvelopeWithLine(envelope, u, true);
		if (intersections.size() == 0 || (intersections.size() == 1 && !borderBottom))
		{
			u += 0.01f;
			continue;
		}
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
			const float maxUDistance = 0.1f;
			float dist = realDist(s, minUV, minUV + glm::fvec2(maxUDistance, 0.0f));
			float dy = 0.1f;// getCurvatureBasedDist(s, minUV, true);
			nextU = u + 0.001f;// dy / dist;
			//nextU = u + realDistToUV(s, getCurvatureBasedDist(s, minUV, true), minUV, minUV + glm::fvec2(1.0f,0.0f));
			if (intersections.size() > 1 && !borderBottom)
			{
				UVEnvelopeIterator& it = intersections[minInd];
				if (!it.GoesInPositiveU())
					it.Reverse();
				glm::fvec2 pos = it.Current();
				while (!it.Finished() && pos.x < nextU)
				{
					uvPositions.push_back(pos);
					glm::fvec2 nextPos = it.Next();
					if (glm::abs(nextPos.x - pos.x) > 0.5f || glm::abs(nextPos.y - pos.y) > 0.5f) // wrap
						break;
					pos = nextPos;
				}
			}
		}
		else
		{
			uvPositions.push_back(minUV);
			uvPositions.push_back(maxUV);
			const float maxUDistance = 0.1f;
			float dist = realDist(s, maxUV, maxUV + glm::fvec2(maxUDistance, 0.0f));
			float dy = 0.1f;// getCurvatureBasedDist(s, minUV, true);
			nextU = u + 0.01f;// dy / dist;
			//nextU = u + realDistToUV(s, getCurvatureBasedDist(s, minUV, true), minUV, minUV + glm::fvec2(1.0f, 0.0f));

			if (intersections.size() > 1)
			{
				UVEnvelopeIterator& it = intersections[maxInd];
				if (!it.GoesInPositiveU())
					it.Reverse();
				glm::fvec2 pos = it.Current();
				while (!it.Finished() && pos.x < nextU)
				{
					uvPositions.push_back(pos);
					glm::fvec2 nextPos = it.Next();
					if (glm::abs(nextPos.x - pos.x) > 0.5f || glm::abs(nextPos.y - pos.y) > 0.5f) // wrap
						break;
					pos = nextPos;
				}
			}
		}
		reverse = !reverse;
		u = nextU;
	}
	return asUV ? uvPathsTo3D(uvPositions) : uvPathsToWorldPaths(s,uvPositions, side);
}

std::vector<glm::fvec3> MillingPathCreator::uvPathsToWorldPaths(std::shared_ptr<IUVSurface> s, std::vector<glm::fvec2> uvPositions, bool side)
{
	std::vector<glm::fvec2> debugPoints;
	float min = 1000000.0f, max = 0.0f; 
	for (int i = 0; i < uvPositions.size(); i++)
	{
		float len = glm::length(s->dfduu(uvPositions[i].x, uvPositions[i].y));
		if (len > max)
			max = len;
		if (len < min)
			min = len;
	}
	std::cout << "d2f/du2 - (" << min << "," << max << ")" << std::endl;
	min = 1000000.0f;
	max = 0.0f;
	for (int i = 0; i < uvPositions.size(); i++)
	{
		float len = glm::length(s->dfdvv(uvPositions[i].x, uvPositions[i].y));
		if (len > max)
			max = len;
		if (len < min)
			min = len;
	}
	std::cout << "d2f/dv2 - (" << min << "," << max << ")" << std::endl;
	float dy;
	//const float acceptAngleCos = glm::cos(std::numbers::pi / 36.0f); // 5st

	std::vector<glm::fvec3> worldPositions = { 10.0f * worldToTargetSpace(s->f(uvPositions[0].x, uvPositions[0].y)) };
	auto addStep = [&worldPositions, &debugPoints, &s](glm::fvec2 uv) {
		worldPositions.push_back(10.0f * worldToTargetSpace(s->f(uv.x, uv.y)));
		debugPoints.push_back(uv);
		};
	int i = 0;
	int prev = 0;
	while (i < uvPositions.size() - 2)
	{
		dy = 0.1f;// getCurvatureBasedDist(s, uvPositions[i], uvPositions, i);
		prev = i;
		// skip identical points
		while (i < uvPositions.size() && realDist(s, uvPositions[i + 1], uvPositions[i]) == 0)
			i++;
		if (i >= uvPositions.size() - 3)
			break;
		
		// skip to close points
		int next = i;
		float dist;
		float angleCos;
		do
		{
			next++;
			dist = realDist(s, uvPositions[i], uvPositions[next + 1]);
			//angleCos = realAngleCos(s, uvPositions[i], uvPositions[i + 1], uvPositions[next + 1]);
		} while (dist < dy /* && angleCos > acceptAngleCos*/ && next < uvPositions.size() - 2);
		i = next;

		// divide distance between distant points 
		glm::fvec2 uvPrev = uvPositions[prev];
		glm::fvec2 uvNext = uvPositions[i];
		glm::fvec2 uv = uvPrev;
		float t = 0;
		while (t < 1)
		{
			dy = 0.1f;// getCurvatureBasedDist(s, uv, uvPositions, prev);
			dist = realDist(s,uvPrev,uvNext);
			t += dy / dist;
			/*
			glm::fvec3 dfdu = s->dfdu(uv.x, uv.y);
			glm::fvec3 dfdv = s->dfdv(uv.x, uv.y);
			glm::fvec3 dfdt = dfdu * (uvNext.x - uvPrev.x) + dfdv * (uvNext.y - uvPrev.y);
			t += dy / dfdt.length();
			*/
			t = glm::clamp(t, 0.0f, 1.0f);
			uv = uvPrev * (1 - t) + uvNext * t;
			
			addStep(uv);
		}
		addStep(uv);
	}
	Debuger::ShowPath(s, debugPoints);
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

std::vector<glm::fvec3> MillingPathCreator::getRoundingPositions(UVEnvelopeIterator& envelopeIt, std::shared_ptr<IUVSurface> surf, bool asUV)
{

	std::vector<glm::fvec2> uvPositions;
	while (!envelopeIt.Finished())
	{
		glm::fvec2 pos = envelopeIt.Next();
		uvPositions.push_back({ pos.x, pos.y });
	}
	return asUV ? uvPathsTo3D(uvPositions) : uvPathsToWorldPathsFlat(surf, uvPositions);
}

std::vector<glm::fvec3> MillingPathCreator::uvPathsTo3D(std::vector<glm::fvec2> uvPositions)
{
	std::vector<glm::fvec3> positions;
	for (auto p : uvPositions)
	{
		positions.push_back({ p.x * 150.0f - 75.0f,p.y * 150.0f - 75.0f,0.0f });
	}
	return positions;
}

std::vector<glm::fvec3> MillingPathCreator::mergePaths(VectorOfReference<std::vector<glm::fvec3>> paths)
{
	std::vector<glm::fvec3> merged;
	for(auto path : paths)
		merged.insert(merged.end(), path.get().begin(), path.get().end());
	return merged;
}

std::vector<glm::fvec3> MillingPathCreator::safeMove(glm::fvec3 from, glm::fvec3 to)
{
	return {
		{from.x,from.y,targetSize.y * 10.0f},
		{to.x,to.y,targetSize.y * 10.0f}
	};
}

float MillingPathCreator::getCurvatureBasedDist(std::shared_ptr<IUVSurface> s, glm::fvec2 pos, std::vector<glm::fvec2>& positions, int i)
{
	bool u = false;
	for (int j = i+1;  j < positions.size(); j++)
	{
		if (pos != positions[j])
		{
			glm::fvec2 uvDist = glm::abs(pos - positions[j]);
			u = uvDist.x > uvDist.y;
			break;
		}
	}
	return getCurvatureBasedDist(s, pos, u);

}

float MillingPathCreator::getCurvatureBasedDist(std::shared_ptr<IUVSurface> s, glm::fvec2 pos, bool u)
{
	float dy;
	glm::fvec3 derivative = u ? s->dfduu(pos.x, pos.y) : s->dfdvv(pos.x, pos.y);
	if (u)
		dy = glm::clamp(1 / (uDistMult * glm::length(derivative) + 0.000001f), 0.1f, 0.5f);
	else
	{
		dy = glm::clamp(1 / (vDistMult * glm::length(derivative) + 0.000001f), 0.1f, 0.5f);
	}
	return dy;
}

float MillingPathCreator::realDistToUV(std::shared_ptr<IUVSurface> s, float d, glm::fvec2 lineStart, glm::fvec2 lineEnd)
{
	glm::fvec3 dfdu = s->dfdu(lineStart.x, lineStart.y);
	glm::fvec3 dfdv = s->dfdv(lineStart.x, lineStart.y);
	glm::fvec3 dfdt = dfdu * (lineEnd.x - lineStart.x) + dfdv * (lineEnd.y - lineStart.y);
	return d / dfdt.length();
}

float MillingPathCreator::realDist(std::shared_ptr<IUVSurface> surf, glm::fvec2 fromUV, glm::fvec2 toUV)
{
	glm::fvec3 from = surf->f(fromUV.x, fromUV.y);
	glm::fvec3 to = surf->f(toUV.x, toUV.y);
	return glm::length(to - from);
}

float MillingPathCreator::realAngleCos(std::shared_ptr<IUVSurface> surf, glm::fvec2 fromUV, glm::fvec2 toUV1, glm::fvec2 toUV2)
{
	glm::fvec3 from = surf->f(fromUV.x, fromUV.y);
	glm::fvec3 to1 = surf->f(toUV1.x, toUV1.y);
	glm::fvec3 to2 = surf->f(toUV2.x, toUV2.y);
	
	auto v1 = glm::normalize(to1 - from);
	auto v2 = glm::normalize(to2 - from);
	if (glm::any(glm::isnan(v1)) || glm::any(glm::isnan(v1)))
		return 1; // there is a loop of close points
	return glm::dot(v1,v2);
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