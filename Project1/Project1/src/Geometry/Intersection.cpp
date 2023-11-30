#include "Intersection.h"
#include <GL/glew.h>
#include <stack>
Indexer Intersection::indexer;

Intersection::Intersection(std::shared_ptr<IUVSurface> s1, std::shared_ptr<IUVSurface> s2, bool cursor, glm::fvec3 cursorPos, float step)
	: s1(s1), s2(s2), name("Intersection-" + std::to_string(indexer.getNewIndex())),
	uvShader("Shaders/2dShader.vert", "Shaders/fragmentShader.frag"), step(step), valid(true)
{
	uvShader.Init();
	initTextures();

	glm::fvec4 beginPoint = cursor ? closestToCursor(cursorPos, 10) : bestSample(10);
	glm::fvec4 p = findIntersectionPoint(beginPoint);
	if (s1.get() == s2.get() && p.x == p.z && p.y == p.w)
	{
		valid = false;
	}
	if (!valid)
		return;
	extendIntersection(p);

	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	
	glViewport(0, 0, 512, 512);
	renderUVTexture(s1, uvS1Fb, uvS1Tex, s1UVLine);
	renderUVTexture(s2, uvS2Fb, uvS2Tex, s2UVLine);
	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
}
 
glm::fvec4 Intersection::wrap(glm::fvec4 x) const
{
	return glm::mod(x + 1.0f, 1.0f);
}

float Intersection::distSqr(glm::fvec4 x)
{
	glm::fvec3 diff = s1->f(x.x,x.y) - s2->f(x.z, x.w);
	return dot(diff, diff);
}

glm::fvec4 Intersection::distGrad(glm::fvec4 x)
{
	glm::fvec3 diff = s1->f(x.x, x.y) - s2->f(x.z, x.w);

	glm::fvec3 dfdu = s1->dfdu(x.x, x.y);
	glm::fvec3 dfdv = s1->dfdv(x.x, x.y);
	glm::fvec3 dgdu = s2->dfdu(x.z, x.w);
	glm::fvec3 dgdv = s2->dfdv(x.z, x.w);

	return 2.0f * glm::fvec4( glm::dot(diff,dfdu), glm::dot(diff,dfdv), 
		     -glm::dot(diff,dgdu),-glm::dot(diff,dgdv) );
}

float Intersection::cursorDist(glm::fvec4 x, glm::fvec3 cursorPosition)
{
	glm::fvec3 l1 = s1->f(x.x, x.y) - cursorPosition;
	glm::fvec3 l2 = s2->f(x.z, x.w) - cursorPosition;
	return glm::dot(l1, l1) + glm::dot(l2, l2);
}
glm::fvec4 Intersection::cursorGrad(glm::fvec4 x, glm::fvec3 cursorPosition)
{
	glm::fvec3 l1 = s1->f(x.x, x.y) - cursorPosition;
	glm::fvec3 l2 = s2->f(x.z, x.w) - cursorPosition;
	return {
		2.0f * glm::dot(l1, s1->dfdu(x.x, x.y)) ,
		2.0f * glm::dot(l1, s1->dfdv(x.x, x.y)),
		2.0f * glm::dot(l2, s2->dfdu(x.z, x.w)),
		2.0f * glm::dot(l2, s2->dfdv(x.z, x.w))
	};
}
glm::fvec4 Intersection::closestToCursor(glm::fvec3 cursorPosition, int division)
{
	glm::fvec4 prevX(2.0f);
	glm::fvec4 x = bestSampleCursor(cursorPosition, division);
	
	
	int it = 0;
	glm::fvec4 grad(1);
	while (glm::dot(x - prevX, x - prevX) > eps * eps)
	{
		grad = cursorGrad(x, cursorPosition);

		float alpha = findAlpha(x, -grad, 
			[this, &cursorPosition](glm::fvec4 x) {return cursorDist(x, cursorPosition); }, 
			[this, &cursorPosition](glm::fvec4 x) {return cursorGrad(x, cursorPosition); });
		prevX = x;
		x = glm::clamp(x - alpha * grad, glm::fvec4(0), glm::fvec4(1));

		it++;
	}
	return x;
}
glm::fvec4 Intersection::bestSampleCursor(glm::fvec3 cursorPosition, int division)
{
	bool selfCross = s1.get() == s2.get();

	glm::fvec4 bestX = { 0, 0, 0, 0 };
	float bestV = cursorDist(bestX,cursorPosition);

	for (int u = 0; u < division; u++)
		for (int v = 0; v < division; v++)
		{
			glm::fvec4 x = { 
				u / (float)(division - 1),
				v / (float)(division - 1),
				0, 0 };
			
			float val = cursorDist(x, cursorPosition);

			if (val < bestV)
			{
				bestV = val;
				bestX = x;
			}
		}

	if (selfCross && bestX.xy() == bestX.wz())
	{
		bestX.z = 1;
		bestV = cursorDist(bestX, cursorPosition);
	}

	for (int u = 0; u < division; u++)
		for (int v = 0; v < division; v++)
		{
			glm::fvec4 x = {
				bestX.xy(),
				u / (float)(division - 1),
				v / (float)(division - 1),
			};

			if (selfCross)
			{
				glm::fvec4 grad = distGrad(x);
				float reverse = (s1->wrappedU() && glm::abs(x.x - x.z) > 0.5f) || (s1->wrappedV() && glm::abs(x.y - x.w) > 0.5f) ? -1 : 1;
				if ((x.x == x.z && x.y == x.w) ||
					(glm::dot(-grad.xy(), x.zw() - x.xy()) * reverse > 0) ||
					(glm::dot(-grad.zw(), x.xy() - x.zw()) * reverse > 0))
					continue;
			}

			float val = cursorDist(x, cursorPosition);

			if (val < bestV)
			{
				bestV = val;
				bestX = x;
			}
		}
	return bestX;
}

glm::fvec4 Intersection::bestSample(int division)
{
	bool selfCross = s1.get() == s2.get();

	glm::fvec4 bestX = {0,0,0.1f,0.1f};
	float bestV = distSqr(bestX);

	float maxU = (selfCross && s1->wrappedU()) ? division - 1 : division;
	float maxV = (selfCross && s1->wrappedV()) ? division - 1 : division;

	for(int s1u = 0; s1u < maxU; s1u++)
		for (int s1v = 0; s1v < maxV; s1v++)
			for (int s2u = 0; s2u < maxU; s2u++)
				for (int s2v = 0; s2v < maxV; s2v++)
				{
					glm::fvec4 x = { s1u,s1v,s2u,s2v };
					x /= (float)(division-1);

					float val = distSqr(x);

					if(selfCross)
					{
						glm::fvec4 grad = distGrad(x);
						float reverse = (s1->wrappedU() && glm::abs(x.x - x.z) > 0.5f) || (s1->wrappedV() && glm::abs(x.y - x.w) > 0.5f) ? -1 : 1;
						if ((s1u == s2u && s1v == s2v) ||
							(glm::dot(-grad.xy(), x.zw() - x.xy()) * reverse > 0) ||
							(glm::dot(-grad.zw(), x.xy() - x.zw()) * reverse > 0))
							continue;
					}

					if (val < bestV)
					{
						bestV = val;
						bestX = x;
					}
				}

	return bestX;
}

float Intersection::findMaxAlpha(glm::fvec4 x, glm::fvec4 dir, bool wrap)
{
	auto maxACond1 = (glm::fvec4(1.0f) - x) / dir;
	auto maxACond2 = -x / dir;
	std::vector<float> max = {
		maxACond1.x, maxACond1.y, maxACond1.z, maxACond1.w,
		maxACond2.x, maxACond2.y, maxACond2.z, maxACond2.w };
	std::vector<bool> wrapped = {
		s1->wrappedU(), s1->wrappedV(), s2->wrappedU(), s2->wrappedV()
	};

	float maxA = maxAlpha;
	for (int i = 0; i < 8; i++)
		if (max[i] >= 0 && maxA > max[i] && (!wrap || !wrapped[i % 4]))
			maxA = max[i];

	return maxA;

}

float Intersection::findAlpha(glm::fvec4 x, glm::fvec4 d, std::function<float(glm::fvec4)> val, std::function<glm::fvec4(glm::fvec4)> grad)
{
	float aL = 0, aR = findAlphaRange(x,d, val);
	
	while (glm::length((aR - aL)*d) > eps)
	{
		float aMiddle = (aL + aR) / 2.0f;
		float dfdam = dot(grad(x + aMiddle * d), d);
		if (fabsf(dfdam) < eps)
			return aMiddle;
		if (dfdam > 0)
			aR = aMiddle;
		else
			aL = aMiddle;
	}
	return (aL + aR) / 2.0f;
}


int Intersection::pixelsInds(int x, int y)
{
	return (y * 512 + x) * 3;
}

void Intersection::floodFill(std::vector<GLbyte>& pixels, glm::ivec2 size, glm::ivec3 color, glm::ivec2 start, glm::bvec2 wrapped)
{
	std::stack<glm::ivec2> points;
	points.push(start);

	do
	{
		glm::ivec2 current = points.top();
		points.pop();
		int ind = pixelsInds(current.x, current.y) + 2;
		if (pixels[ind] == GLbyte(255))
			continue;
		pixels[ind] = GLbyte(255);

		glm::ivec2 px = { (current.x - 1 + size.x) % size.x, current.y };
		glm::ivec2 nx = { (current.x + 1) % size.x, current.y };
		glm::ivec2 py = { current.x, (current.y - 1 + size.y) % size.y };
		glm::ivec2 ny = { current.x, (current.y + 1) % size.y };

		if ( pixels[pixelsInds(px.x,py.y)] != GLbyte(255) && (wrapped.x || current.x > 0))
			points.push(px);
		if (pixels[pixelsInds(nx.x, nx.y)] != GLbyte(255) && (wrapped.x || current.x < (size.x - 1)))
			points.push(nx);
		
		if (pixels[pixelsInds(py.x, py.y)] != GLbyte(255) && (wrapped.y || current.y > 0))
			points.push(py);
		if (pixels[pixelsInds(ny.x, ny.y)] != GLbyte(255) && (wrapped.y || current.y < (size.y - 1)))
			points.push(ny);
		
	} while (!points.empty());
}

glm::fvec4 Intersection::findIntersectionPoint(glm::fvec4 beginPoint)
{
	glm::fvec4 x = beginPoint;

	int it = 0;
	float val = distSqr(x);
	auto prevGrad = distGrad(x);
	auto d = -prevGrad;
	while (fabsf(val)>eps )
	{
		if (it > 10000)
		{
			valid = false;
			return glm::fvec4(0);
		}
		it++;
		float alpha = findAlpha(x, d,
			[this](glm::fvec4 x) {return distSqr(x); },
			[this](glm::fvec4 x) {return distGrad(x); });
		x  = wrap(x + alpha * d);
		val = distSqr(x);

		auto grad = distGrad(x);
		float beta = glm::dot(grad,grad)/glm::dot(prevGrad,prevGrad);
		d = -grad +beta * d;
		prevGrad = grad;

	}
	return x;
}

float Intersection::findAlphaRange(glm::fvec4 x, glm::fvec4 d, std::function<float(glm::fvec4)> val)
{
	float maxA = findMaxAlpha(x,d);
	if (maxA == 0)
		return 0;
	float delta = maxA / 100.0f;
	float a2 = delta;
	
	float f1 = val(x),
		  f2 = val(x + a2 * d);
	if (f2 > f1)
		return a2;
	
	while (true)
	{
		float a3 = a2 + delta;
		if (a3 > maxA)
			return maxA;

		float f3 = val(x + a3 * d);
		if (f3 > f2)
			return a3;

		delta *= 2.0f;
	}
		
}

glm::mat4x4 Intersection::jacoby(glm::fvec4 x, glm::fvec3 d) const
{
	glm::fvec3 dfu = s1->dfdu(x.x, x.y);
	glm::fvec3 dfv = s1->dfdv(x.x, x.y);
	glm::fvec3 dgu = s2->dfdu(x.z, x.w);
	glm::fvec3 dgv = s2->dfdv(x.z, x.w);

	return {
		 dfu.x, dfu.y, dfu.z, glm::dot(dfu,d),
		 dfv.x, dfv.y, dfv.z, glm::dot(dfv,d),
		-dgu.x,-dgu.y,-dgu.z, 0,
		-dgv.x,-dgv.y,-dgv.z, 0
	};
}

std::tuple<std::vector<glm::fvec4>, bool> Intersection::findIntersectionPointsInDirection(glm::fvec4 x, bool dir)
{
	glm::bvec4 wrapped = {
		s1->wrappedU(), s1->wrappedV(), s2->wrappedU(), s2->wrappedV()
	};
	std::vector<glm::fvec4> p;
	glm::fvec4 xPrev;
	glm::fvec3 begin = s1->f(x.x,x.y);
	float dirSgn = dir ? 1 : -1;

	int i = 0;
	do
	{
		if (i > 100000) return { p, false };
		i++;
		glm::fvec3 dfu = s1->dfdu(x.x, x.y);
		glm::fvec3 dfv = s1->dfdv(x.x, x.y);
		glm::fvec3 dgu = s2->dfdu(x.z, x.w);
		glm::fvec3 dgv = s2->dfdv(x.z, x.w);

		glm::fvec3 n1 = glm::cross(dfu, dfv);
		glm::fvec3 n2 = glm::cross(dgu, dgv);
		if (glm::all(glm::lessThan(glm::abs(glm::normalize(n1) - glm::normalize(n2)), glm::fvec3(eps))))
			return { p, false };
		glm::fvec3 d = dirSgn * normalize(glm::cross(n1, n2));

		xPrev = x;
		glm::fvec4 val = glm::fvec4(0, 0, 0, -step);

		int it = 0;
		while (glm::any(glm::greaterThan(glm::abs(val), glm::fvec4(eps))) && it<5)
		{
			it++;
			auto J = glm::inverse(jacoby(x, d));

			auto diff = J * val;
			x = x - diff;
			if (glm::any(glm::lessThan(x, glm::fvec4(0)) && glm::not_(wrapped)) ||
				glm::any(glm::greaterThan(x, glm::fvec4(1)) && glm::not_(wrapped)))
			{
				x = x + diff;
				float a = findMaxAlpha(x, -diff);
				p.push_back(glm::clamp(x - a * diff,0.0f,1.0f));
				return { p, false };
			}
			x = wrap(x);
			val = { s1->f(x.x,x.y) - s2->f(x.z,x.w), glm::dot(s1->f(x.x,x.y) - s1->f(xPrev.x,xPrev.y),d) - step };
		}
		p.push_back(x);
	} while (glm::length(begin - s1->f(x.x,x.y)) > step / 2.0f);

	glm::fvec4 loopDir = p[0] - x;
	if(glm::length(loopDir) < 0.5)
		p.push_back(p[0]);
	else
	{
		float a = findMaxAlpha(x, -loopDir, false);
		p.push_back(x - a * loopDir);
		
		a = findMaxAlpha(p[0], loopDir, false);
		p.insert(p.begin(), p[0] + a * loopDir);
		
	}
	return { p, true };
}

void Intersection::extendIntersection(glm::fvec4 x)
{
	std::vector<glm::fvec4> uvs;
	std::vector<glm::fvec3> points;
	std::vector<glm::fvec3> s1uvs;
	std::vector<glm::fvec3> s2uvs;

	auto [p1,loop1] = findIntersectionPointsInDirection(x, true);
	if(!loop1)
	{
		auto [p2, loop2] = findIntersectionPointsInDirection(x, false);
		uvs.insert(uvs.end(), p2.rbegin(), p2.rend());
	}
	uvs.insert(uvs.end(), p1.begin(),  p1.end());
	
	for (auto p : uvs)
	{
		points.push_back(s1->f(p.x, p.y));
		s1uvs.push_back(glm::fvec3(p.xy(), 0));
		s2uvs.push_back(glm::fvec3(p.zw(), 0));
	}


	sceneCurve.setPoints(points);
	s1UVLine.setPoints(s1uvs, true);
	s2UVLine.setPoints(s2uvs, true);
}

std::string Intersection::getName() const
{
	return name;
}

void Intersection::Render(bool selected, VariableManager& vm)
{
	sceneCurve.Render(selected, vm);

	for(auto& v : debugVec)
		v->Render(selected, vm);
}

bool Intersection::RenderGui()
{
	ImVec2 size{ 400,400 };
	ImGui::Begin("Intersection");
	if (ImGui::Button("Make Curve"))
	{
		std::vector<std::shared_ptr<Point>> points;
		auto linePoints = sceneCurve.getPoints();
		std::transform(linePoints.begin(), linePoints.end(), std::back_inserter(points), [](const glm::fvec3& pos) {
			return std::make_shared<Point>(glm::fvec4(pos,1.0f));
			});
		toAdd.push_back(std::make_shared<InterpolationCurve>(points));
		toAdd.insert(toAdd.end(), points.begin(), points.end());
	}
	ImGui::Image((ImTextureID)uvS1Tex, size);
	ImGui::Image((ImTextureID)uvS2Tex, size);
	ImGui::End();
	return false;
}

void Intersection::initTextures()
{
	glGenFramebuffers(1, &uvS1Fb);
	glBindFramebuffer(GL_FRAMEBUFFER, uvS1Fb);

	glGenTextures(1, &uvS1Tex);
	glBindTexture(GL_TEXTURE_2D, uvS1Tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uvS1Tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &uvS2Fb);
	glBindFramebuffer(GL_FRAMEBUFFER, uvS2Fb);

	glGenTextures(1, &uvS2Tex);
	glBindTexture(GL_TEXTURE_2D, uvS2Tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uvS2Tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Intersection::renderUVTexture(std::shared_ptr<IUVSurface> s, unsigned int uvFb, unsigned int uvTex, BrokenLine& line)
{
	VariableManager vm;
	vm.AddVariable("color", glm::fvec4(1, 1, 1, 1));

	glBindFramebuffer(GL_FRAMEBUFFER, uvFb);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	uvShader.use();
	vm.Apply(uvShader.ID);
	line.Render(false, vm);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<GLbyte> col(512 * 512 * 3, 0);
	glBindTexture(GL_TEXTURE_2D, uvTex);

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, &col[0]);

	floodFill(col, { 512,512 }, { 255,255,255 }, { 0,0 }, { s->wrappedU(), s->wrappedV() });

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 512, 512, GL_RGB, GL_UNSIGNED_BYTE, &col[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Intersection::onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
}

void Intersection::onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
	if (elem.get() == this)
	{
		auto sharedThis = std::dynamic_pointer_cast<Intersection>(elem);
		s1->removeIntersection(sharedThis);
		s2->removeIntersection(sharedThis);
	}
}

void Intersection::onCollapse(Scene& scene, std::vector<std::shared_ptr<Point>>& collapsed, std::shared_ptr<Point> result)
{
}

void Intersection::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
}

void Intersection::onMove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
}

std::vector<std::shared_ptr<ISceneElement>> Intersection::GetAddedObjects()
{
	auto ret = toAdd;
	toAdd.clear();
	return ret;
}

std::vector<std::shared_ptr<ISceneElement>> Intersection::GetRemovedObjects()
{
	return {};
}

UVCurve Intersection::GetUVCurve(std::shared_ptr<IUVSurface> surface) const
{
	auto pointsSrc = (surface.get() == s1.get() ? s1UVLine : s2UVLine).getPoints();
	std::vector<glm::fvec2> pointsDest;
	std::transform(pointsSrc.begin(), pointsSrc.end(), std::back_inserter(pointsDest), [](const glm::fvec3& p) {return glm::fvec2( p.x,p.y ); });
	return UVCurve(pointsDest);
}

std::shared_ptr<IUVSurface> Intersection::GetOtherSurface(std::shared_ptr<IUVSurface> thisSurf) const
{
	return s1.get() == thisSurf.get() ? s2 : s1;
}

unsigned int Intersection::GetTextureForSurface(std::shared_ptr<IUVSurface> surface) const
{
	return s1.get() == surface.get() ? uvS1Tex : uvS2Tex;
}
