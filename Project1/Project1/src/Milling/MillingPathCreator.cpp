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
const float MillingPathCreator::targetBaseHeight = 1.5f;
const float MillingPathCreator::epsilon = 0.001f;
const float MillingPathCreator::roughtPathsTranslation = 0.5f;
float MillingPathCreator::uDistMult = 0.0f;
float MillingPathCreator::vDistMult = 1.0f;
float MillingPathCreator::minDetailDist = 0.01f;
float MillingPathCreator::maxDetailDist = 0.5f;
float MillingPathCreator::frequencyMultipier = 1.0f;
float MillingPathCreator::dencityMultipier = 0.2f;  
float MillingPathCreator::frequecnyCurvatureInfluence = 0.0f;
float MillingPathCreator::dencityCurvatureInfluence = 0.0f;


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

std::vector<glm::fvec3> MillingPathCreator::mergePaths(VectorOfReference<std::vector<glm::fvec3>> paths)
{
	std::vector<glm::fvec3> merged;
	for (auto path : paths)
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

float MillingPathCreator::targetToWorldSpace(float dist)
{
	return dist * 3.0f;
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

glm::fvec3 MillingPathCreator::applyBase(glm::fvec3 pos)
{
	pos += glm::fvec3(0.0f, 0.0f, targetBaseHeight * 10.0f);
	pos.z = glm::max(15.0f, pos.z);
	return pos;
}

unsigned int MillingPathCreator::renderSceneHeightMap(Scene& scene)
{
	float r = targetToWorldSpace(0.8f);
	std::shared_ptr<SurfaceShift> handle = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "handle"), r, true);
	std::shared_ptr<SurfaceShift> body = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "body"), r, false);
	std::shared_ptr<SurfaceShift> button = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "button"), r, false,true);
	
	TextureRenderer tr(textureSize, textureSize, 3, true);

	float rangeLen = zRange.y - zRange.x;

	// setup uniforms
	VariableManager vm;
	vm.AddVariable("projMtx", glm::ortho<float>(rect.x,rect.y,rect.z, rect.w, 0.001f, fabsf(rangeLen)*2));
	vm.AddVariable("viewMtx", glm::translate(glm::rotate<float>(glm::identity<glm::fmat4x4>(), std::numbers::pi, { 0,1,0 }), { 0,0,-rangeLen }));
	vm.AddVariable("modelMtx", glm::identity<glm::fmat4x4>());
	vm.AddVariable("heightRange", zRange);

	// render
	tr.Clear({ 0,0,0,0 });
	tr.Render(*handle, vm);
	tr.Render(*body, vm);
	tr.Render(*button, vm);
	
	return tr.getTextureId();
}

std::vector<glm::fvec3> MillingPathCreator::sampleHeigthMap(unsigned int heightMap, float level, bool startTopLeft)
{
	float cadWidth = rect.y - rect.x;
	float cadHeight = fabsf(zRange.x - zRange.y);
	float materialLimitInCadSpace = targetSize.y * cadWidth / targetSize.x;
	float materialLimitInTextureSpace = materialLimitInCadSpace / cadHeight;
	float levelInTextureSpace = level * materialLimitInTextureSpace;
	float levelInTargetSpace = level * targetSize.y;

	TextureCPUAccess textureCPU(heightMap,textureSize,textureSize);

	std::vector<glm::fvec3> positions;
	float worldR = targetToWorldSpace(0.8f);
	float roughEps = worldR/10.0f;
	float pathWidth = worldR;
	int pathCount = ceilf(cadWidth / pathWidth) + 1;
	int pathDivision = 100;

	positions.reserve(pathCount * pathDivision);
	for (int i = 0; i < pathCount; i++)
	{
		for (int j = 0; j < pathDivision; j++)
		{
			float u = i / (float)(pathCount - 1);
			float v = j / (float)(pathDivision - 1);
			if (i % 2 == 0)
				v = 1 - v;
			glm::fvec3 positionTargetSpace = {
				u * targetSize.x - targetSize.x / 2.0f,
				v * targetSize.z - targetSize.z / 2.0f,
				0 };
			float heightTexSpace = textureCPU.getByUV({ u,1 - v }).x;
			float heightTargetSpace = targetSize.y * heightTexSpace / materialLimitInTextureSpace;
			positionTargetSpace.z = heightTexSpace < levelInTextureSpace ? levelInTargetSpace : heightTargetSpace;
			if (j == 0 || positionTargetSpace.y != positions.rbegin()->y)
				positions.push_back(applyBase(positionTargetSpace * 10.0f));
		}
	}
	if (!startTopLeft)  std::reverse(positions.begin(), positions.end());
	return positions;
}

MillingPath MillingPathCreator::CreateRoughtingPath(Scene& scene)
{
	unsigned int tex = renderSceneHeightMap(scene);
	Debuger::ShowTexture(tex);

	float halfHeight = (targetSize.y - targetBaseHeight) / targetSize.y / 2.0f;
	std::vector<glm::fvec3> positions1 = sampleHeigthMap(tex, halfHeight, true);
	std::vector<glm::fvec3> positions2 = sampleHeigthMap(tex, 0.1f, false);
	std::vector<glm::fvec3> startPoint = { {0.0f, 0.0f, 60.0f } };
	std::vector<glm::fvec3> positions = mergePaths({
		startPoint,
		positions1,
		positions2,
		startPoint
		});
	return MillingPath(positions, 16, false);
}

MillingPath MillingPathCreator::CreateBasePath(Scene& scene)
{
	const float r = targetToWorldSpace(0.6f);
	std::shared_ptr<IUVSurface> base = findSurfaceByName(scene, "base");
	std::shared_ptr<IUVSurface> handle = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "handle"), r, true);
	std::shared_ptr<IUVSurface> body = std::make_shared<SurfaceShift>(findSurfaceByName(scene, "body"), r, false);

	intersect(scene, base, handle, { 0, 2, -r });
	intersect(scene, body, base, { 0, -10, -r });

	UVEnvelope envelope = createEnvelope(base);
	
	std::vector<glm::fvec3> positions1 = uvPathsToWorldPathsFlat(base, millBase(base, envelope, 0.6f, false));
	std::vector<glm::fvec3> positions2 = uvPathsToWorldPathsFlat(base, millBase(base, envelope, 0.6f, true));
	UVEnvelopeIterator it(envelope);
	auto roundingPositions = getRoundingPositions(it, base);
	
	auto transPos1 = 10.0f * worldToTargetSpace(base->f(0, 1));
	glm::fvec3 transPos2 = { transPos1.x, roundingPositions.back().y, transPos1.z};
	std::vector<glm::fvec3> safeTransition = { transPos1, transPos2 };
	std::vector<glm::fvec3> positions = mergePaths({ positions1,positions2,safeTransition, roundingPositions });
	for (int i = 0; i<positions.size(); i++)
	{
		//positions[i].z = 0;
		positions[i] = applyBase(positions[i]);
	}

	std::vector<glm::fvec3> startPoints = { {0.0f, 0.0f, 60.0f }, {0.0f,-90.0f, 60.0f}, applyBase({0.0f,-90.0f, 0.0f}) };
	std::vector<glm::fvec3> endPoint = { { positions.back().x, positions.back().y, 60.0f}, {0.0f, 0.0f, 60.0f } };
	positions = mergePaths({
		startPoints,
		positions,
		endPoint
		});

	return MillingPath(positions, 12, true);
}

std::vector<glm::fvec2> MillingPathCreator::millBase(std::shared_ptr<IUVSurface> surf, UVEnvelope& envelope, float r, bool direction)
{
	r = targetToWorldSpace(r);
	std::vector<glm::fvec2> positionsUV;
	float cadWidth = rect.y - rect.x;
	float roughEps = r / 10.0f;
	float pathWidth = 2 * r - 3 * roughEps;
	int pathCount = ceilf(cadWidth / pathWidth) + 1;
	int pathDivision = 100;
	bool reverse = false;
	for (int i = 0; i < pathCount; i++)
	{
		float v = i / (float)(pathCount - 1);
		if (!direction)
			v = 1 - v;
		if (i == 0)
		{
			positionsUV.push_back({ 1, v });
			positionsUV.push_back({ 0, v });
			continue;
		}
		positionsUV.push_back({ positionsUV.rbegin()->x, v });

		auto intersections = intersectEnvelopeWithLine(envelope, v, false);
		if (intersections.empty() || i % 2 == (direction ? 1 : 0))
		{
			positionsUV.push_back({ reverse ? 0 : 1, v });
		}
		else
		{
			std::vector<glm::fvec2> envelopePointsUV;

			glm::fvec2 minInt = { direction ? 0 : 1, v };
			int minI = 0;
			for (int j = 0; j < intersections.size(); j++)
			{
				glm::fvec2 intUV = intersections[j].second.Current();
				if ((intUV.x < minInt.x) != direction)
				{
					minI = j;
					minInt = intUV;
				}
			}
			envelopePointsUV.push_back({ minInt.x, minInt.y });

			UVEnvelopeIterator& it = intersections[minI].second;
			glm::fvec2 pos = it.Current();
			float nextV = (i + 1) / (float)(pathCount - 1);
			if (!direction)
				nextV = 1 - nextV;

			while (!it.Finished() && (pos.y > nextV) != direction)
			{
				glm::fvec2 nextPos = it.Next();
				if ((pos.y < nextPos.y) != direction || (nextPos.y <= nextV) != direction)
					break;
				pos = nextPos;
				envelopePointsUV.push_back({ pos.x, pos.y });
			}
			auto skipped = skipUnneadedPoints(surf, envelopePointsUV, 0.2f, false);
			positionsUV.insert(positionsUV.end(), skipped.begin(), skipped.end());
		}
		reverse = !reverse;
	}
	return positionsUV;
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
	std::vector<glm::fvec2> uvPositions; // Debug only

	std::vector<glm::fvec3> positions;
	glm::fvec2 last = envelopeIt.Current();
	glm::fvec2 pos = envelopeIt.Current();
	while (!envelopeIt.Finished())
	{
		last = pos;
		pos = envelopeIt.Next();
		glm::fvec2 dif = glm::abs(pos - last);
		if ((dif.x > 0.1f && dif.x<0.7f) || (dif.y > 0.1f && dif.y < 0.7f))
		{
			for (glm::fvec3 p : safeMove(10.0f * worldToTargetSpace(surf->f(last.x, last.y)), 10.0f * worldToTargetSpace(surf->f(pos.x, pos.y))))
				positions.push_back(p);
		}
		positions.push_back(10.0f * worldToTargetSpace(surf->f(pos.x, pos.y)));

		uvPositions.push_back({ pos.x, pos.y }); // Debug only
	}
	//Debuger::ShowPath(surf, uvPositions);
	return asUV ? uvPathsTo3D(uvPositions) : positions;
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

	/*Debuger::ShowUVSurface(body);
	Debuger::ShowUVSurface(handle);
	Debuger::ShowUVSurface(base);
	Debuger::ShowUVSurface(button);*/

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
	
	auto positionsBody = millUVSurface(body, envelopeBody, false, false, false, { 
		{0,0}, 
		{0.12f, 0.5f}, {0.25f, 0.0f}, 
		{0.35f, -0.2f}, {0.45f,0.0f}, 
		{0.55f, 0.5f}, {0.68f,0.0f}, 
		{0.8f, 1.5f} },0.0028f);
	auto positionsHandle = millUVSurface(handle, envelopeHandle, false, true, false, {
		{0,0},
		{0.08f, 0.4f}, {0.27f, 0.0f },
		{ 0.73f, 0.4f},{0.92f, 0.0f}
		}, 0.005f);
	auto positionsHole = millUVSurface(base, envelopeHole, false, false, false, { {0,0} }, 0.006f);
	auto positionsHole1 = millUVSurface(base, envelopeHole, false, true, false, { {0,0} }, 0.006f);
	auto positionsButton = millUVSurface(button, envelopeButton, true, false, false, { {0,0} }, 0.006f);
	
	UVEnvelopeIterator bodyIt(envelopeBody);
	UVEnvelopeIterator handleIt(envelopeHandle);
	auto roundingBody = getRoundingPositions(bodyIt, body);
	auto roundingHandle = getRoundingPositions(handleIt, handle);
	
	auto positionsBodyToHandle = safeMove(positionsBody.back(), positionsHandle.front());
	auto positionsHandleToHole = safeMove(positionsHandle.back(), positionsHole.front());
	auto positionsHoleToButton = safeMove(positionsHole1.back(), positionsButton.front());
	
	auto positionsButtonToRoundingBody = safeMove(positionsButton.back(), roundingBody.front());
	auto roundingBodyToRoundingHandle = safeMove(roundingBody.back(), roundingHandle.front());
	
	UVEnvelopeIterator it(envelopeBody);
	//std::vector<glm::fvec3> positionsBodyRounding = getRoundingPositions(it, body, true);
	//std::vector<glm::fvec3> positions1 = millUVSurface(handle, envelopeHandle,false,true,true);
	//for (int i = 0; i < positions1.size() / 2.0f; i++)
	//	positions.push_back(positions1[i]);

	std::vector<glm::fvec3> startPoint = { {0.0f, 0.0f, 60.0f } };
	std::vector<glm::fvec3> positions = mergePaths({
			startPoint,
			
			positionsBody,
			positionsBodyToHandle,
			positionsHandle,
			positionsHandleToHole,
			positionsHole,
			positionsHole1,
			positionsHoleToButton,
		    positionsButton,
			
			positionsButtonToRoundingBody,
			roundingBody,
			roundingBodyToRoundingHandle,
			roundingHandle,
			
			startPoint
		});
	for (int i = 0; i < positions.size(); i++)
	{
		positions[i] = applyBase(positions[i] - glm::fvec3(0.0f, 0.0f, 4.0f));
	}
	return MillingPath(positions, 4, false);
}

std::vector<glm::fvec3> MillingPathCreator::millUVSurface(std::shared_ptr<IUVSurface> s, UVEnvelope& envelope, bool borderBottom, bool side, bool asUV, std::vector<std::pair<float, float>> keyDistMultipliers, float frequency)
{
	std::vector<glm::fvec2> uvPositions;
	bool reverse = false;
	float u = uDistMult;
	auto reverseUV = [](glm::fvec2 uv) { return glm::fvec2(uv.y, uv.x); };
	keyDistMultipliers.emplace_back(1, 0);
	while (u <= vDistMult)
	{
		float distMult = 1.0f;
		for (int i = 1; i < keyDistMultipliers.size(); i++)
		{
			if (keyDistMultipliers[i].first > u)
			{
				float u0 = keyDistMultipliers[i - 1].first;
				float u1 = keyDistMultipliers[i].first;
				distMult = 1.0f - 4.0f / (u0 - u1) / (u0 - u1) * keyDistMultipliers[i - 1].second * (u - u0) * (u - u1);
				break;
			}
		}
		float nextU;
		auto intersections = intersectEnvelopeWithLine(envelope, u, !side);
		if (intersections.size() == 0 || (intersections.size() == 1 && !borderBottom))
		{
			u += 0.01f;
			continue;
		}
		int minInd = -1;
		int maxInd = -1;
		glm::fvec2 minUV = { u, borderBottom ? 0 : 1 };
		glm::fvec2 maxUV = { u, 0 };
		if (side)
		{
			minUV = reverseUV(minUV);
			maxUV = reverseUV(maxUV);
		}
		for (int j = 0; j < intersections.size(); j++)
		{
			glm::fvec2 current = intersections[j].first;
			if ((!side && current.y < minUV.y) || (side && current.x < minUV.x))
			{
				minUV = current;
				minInd = j;
			}
			if ((!side && current.y > maxUV.y) || (side && current.x > maxUV.x))
			{
				maxUV = current;
				maxInd = j;
			}
		}
		if (side)
		{
			maxUV.x -= 0.005f;
			minUV.x += 0.005f;
			maxUV.y = u;
			minUV.y = u;
		}
		else
		{
			maxUV.y -= 0.005f;
			minUV.y += 0.005f;
			maxUV.x = u;
			minUV.x = u;
		}
		if (reverse)
		{
			uvPositions.push_back(maxUV);
			uvPositions.push_back(minUV);
			const float maxUDistance = 0.1f;
			float dist = realDist(s, minUV, minUV + (side ? glm::fvec2(0.0f,maxUDistance) : glm::fvec2(maxUDistance, 0.0f)));
			if (dist < 1)
				dist = 1.0f;
			float dy = frequencyMultipier * getCurvatureBasedDist(s, minUV, true, frequecnyCurvatureInfluence);
			nextU = u + frequency * distMult;// dy / dist;
			//nextU = u + realDistToUV(s, getCurvatureBasedDist(s, minUV, true), minUV, minUV + glm::fvec2(1.0f,0.0f));
			/*
			if (intersections.size() > 1 && !borderBottom && !side)
			{
				UVEnvelopeIterator& it = intersections[minInd];
				if (!it.GoesInPositiveU())
					it.Reverse();
				glm::fvec2 pos = it.Current();
				while (!it.Finished() && ((!side && pos.x < nextU) || (side && pos.y < nextU)))
				{
					//uvPositions.push_back(pos);
					glm::fvec2 nextPos = it.Next();
					if (glm::abs(nextPos.x - pos.x) > 0.5f || glm::abs(nextPos.y - pos.y) > 0.5f) // wrap
						break;
					pos = nextPos;
				}
				uvPositions.push_back(pos);
			}
			*/
		}
		else
		{
			uvPositions.push_back(minUV);
			uvPositions.push_back(maxUV);
			const float maxUDistance = 0.1f;
			float dist = realDist(s, maxUV, maxUV + (side ? glm::fvec2(0.0f, maxUDistance) : glm::fvec2(maxUDistance, 0.0f)));
			if (dist == 0)
				dist = 1.0f;
			float dy = frequencyMultipier * getCurvatureBasedDist(s, minUV, true, frequecnyCurvatureInfluence);
			nextU = u + frequency * distMult;// dy / dist;
			//nextU = u + realDistToUV(s, getCurvatureBasedDist(s, minUV, true), minUV, minUV + glm::fvec2(1.0f, 0.0f));
			/*
			if (intersections.size() > 1 && !side)
			{
				UVEnvelopeIterator& it = intersections[maxInd];
				if (!it.GoesInPositiveU())
					it.Reverse();
				glm::fvec2 pos = it.Current();
				while (!it.Finished() && ((!side && pos.x < nextU) || (side && pos.y < nextU)))
				{
					//uvPositions.push_back(pos);
					glm::fvec2 nextPos = it.Next();
					if (glm::abs(nextPos.x - pos.x) > 0.5f || glm::abs(nextPos.y - pos.y) > 0.5f) // wrap
						break;
					pos = nextPos;
				}
				uvPositions.push_back(pos);
			}
			*/
		}
		reverse = !reverse;
		if (u == 1.0f)
			break;
		u = glm::clamp(nextU,0.0f,1.0f);
	}
	return asUV ? uvPathsTo3D(uvPositions) : uvPathsToWorldPaths(s,uvPositions, side);
}

std::vector<glm::fvec2> MillingPathCreator::skipUnneadedPoints(std::shared_ptr<IUVSurface> s, std::vector<glm::fvec2> points, float minD, bool useCurvature)
{
	std::vector<glm::fvec2> outputPoints = { points.front() };
	int i = 0;
	int prev = 0;
	while (i < points.size() - 2)
	{
		float d = useCurvature ? getCurvatureBasedDist(s, points[i], points, i, dencityCurvatureInfluence) : minD;
		prev = i;
		// skip identical points
		while (i < points.size() && realDist(s, points[i + 1], points[i]) == 0)
			i++;
		if (i >= points.size() - 3)
			break;

		// skip to close points
		int next = i;
		float dist;
		float angleCos;
		do
		{
			next++;
			dist = realDist(s, points[i], points[next + 1]);
			//angleCos = realAngleCos(s, uvPositions[i], uvPositions[i + 1], uvPositions[next + 1]);
		} while (dist < d /* && angleCos > acceptAngleCos*/ && next < points.size() - 2);
		i = next;
		outputPoints.push_back(points[i]);
	}
	outputPoints.push_back(points.back());
	return outputPoints;
}

float MillingPathCreator::getCurvatureBasedDist(std::shared_ptr<IUVSurface> s, glm::fvec2 pos, std::vector<glm::fvec2>& positions, int i, float influence)
{
	bool u = false;
	for (int j = i + 1; j < positions.size(); j++)
	{
		if (pos != positions[j])
		{
			glm::fvec2 uvDist = glm::abs(pos - positions[j]);
			u = uvDist.x > uvDist.y;
			break;
		}
	}
	return getCurvatureBasedDist(s, pos, u, influence);

}

float MillingPathCreator::getCurvatureBasedDist(std::shared_ptr<IUVSurface> s, glm::fvec2 pos, bool u, float influence)
{
	float dy;
	glm::fvec3 derivative = u ? s->dfduu(pos.x, pos.y) : s->dfdvv(pos.x, pos.y);
	if (u)
		dy = glm::clamp(maxDetailDist / glm::pow(glm::length(derivative) + 0.000001f,influence), minDetailDist, maxDetailDist);
	else
	{
		dy = glm::clamp(maxDetailDist / glm::pow(glm::length(derivative) + 0.000001f, influence), minDetailDist, maxDetailDist);
	}
	return dy;
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
		dy = dencityMultipier * getCurvatureBasedDist(s, uvPositions[i], uvPositions, i, dencityCurvatureInfluence);
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
			dy = dencityMultipier * getCurvatureBasedDist(s, uv, uvPositions, prev, dencityCurvatureInfluence);
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

std::vector<glm::fvec3> MillingPathCreator::uvPathsTo3D(std::vector<glm::fvec2> uvPositions)
{
	std::vector<glm::fvec3> positions;
	for (auto p : uvPositions)
	{
		positions.push_back({ p.x * 150.0f - 75.0f,p.y * 150.0f - 75.0f,0.0f });
	}
	return positions;
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

std::vector<std::pair<glm::fvec2, UVEnvelopeIterator>> MillingPathCreator::intersectEnvelopeWithLine(UVEnvelope& envelope, float t, bool u)
{
	std::vector<std::pair<glm::fvec2, UVEnvelopeIterator>> positions;
	UVEnvelopeIterator it(envelope);
	glm::fvec2 last = it.Current();
	while (!it.Finished())
	{
		glm::fvec2 current = it.Next();
		if (fabsf(current.x - last.x) < 0.5f && fabsf(current.y - last.y) < 0.5f)
		{
			if ((u && ((current.x >= t && last.x < t) || (current.x <= t && last.x > t))) ||
				(!u && ((current.y >= t && last.y < t) || (current.y <= t && last.y > t))))
			{
				if (u)
				{
					float ct = (t - last.x) / (current.x - last.x);
					positions.emplace_back(last * (1 - ct) + current * ct, UVEnvelopeIterator(it));
				}
				else
				{
					float ct = (t - last.y) / (current.y - last.y);
					positions.emplace_back(last * (1 - ct) + current * ct, UVEnvelopeIterator(it));
				}
			}
		}
		else
		{
			glm::fvec2 min = glm::min(current, last);
			glm::fvec2 max = glm::max(current, last);
			if ((u && (t <= min.x || t >= max.x)) ||
				(!u && (t <= min.y || t >= max.y)))
				positions.emplace_back(current, UVEnvelopeIterator(it));
		}
		last = current;
	}
	return positions;
}