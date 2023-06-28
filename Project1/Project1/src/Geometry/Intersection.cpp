#include "Intersection.h"
#include <GL/glew.h>
#include <stack>
Indexer Intersection::indexer;

glm::fvec4 Intersection::wrap(glm::fvec4 x) const
{
	return glm::mod(x + 1.0f, 1.0f);
}

float Intersection::distSqr(glm::fvec4 x)
{
	x = wrap(x);
	glm::fvec3 diff = s1->f(x.x,x.y) - s2->f(x.z, x.w);
	return dot(diff, diff);
}

glm::fvec4 Intersection::distGrad(glm::fvec4 x)
{
	x = wrap(x);
	glm::fvec3 diff = s1->f(x.x, x.y) - s2->f(x.z, x.w);

	glm::fvec3 dfdu = s1->dfdu(x.x, x.y);
	glm::fvec3 dfdv = s1->dfdv(x.x, x.y);
	glm::fvec3 dgdu = s2->dfdu(x.z, x.w);
	glm::fvec3 dgdv = s2->dfdv(x.z, x.w);

	return 2.0f * glm::fvec4( glm::dot(diff,dfdu), glm::dot(diff,dfdv), 
		     -glm::dot(diff,dgdu),-glm::dot(diff,dgdv) );
}

float Intersection::findAlpha(glm::fvec4 x, glm::fvec4 d)
{
	float aL = 0, aR = findAlphaRange(x,d);
	
	while (glm::length((aR - aL)*d) > eps)
	{
		float aMiddle = (aL + aR) / 2.0f;
		float dfdam = dot(distGrad(x + aMiddle * d), d);
		if (fabsf(dfdam) < eps)
			return aMiddle;
		if (dfdam > 0)
			aR = aMiddle;
		else
			aL = aMiddle;
	}
	return (aL + aR) / 2.0f;
}

Intersection::Intersection(std::shared_ptr<IUVSurface> s1, std::shared_ptr<IUVSurface> s2)
	: s1(s1), s2(s2), name("Intersection-" + std::to_string(indexer.getNewIndex())),
	uvShader("Shaders/2dShader.vert", "Shaders/fragmentShader.frag")
{
	uvShader.Init();

	glGenFramebuffers(1, &uvS1Fb);
	glBindFramebuffer(GL_FRAMEBUFFER, uvS1Fb);

	glGenTextures(1, &uvS1Tex);
	glBindTexture(GL_TEXTURE_2D, uvS1Tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uvS1Tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &uvS2Fb);
	glBindFramebuffer(GL_FRAMEBUFFER, uvS2Fb);

	glGenTextures(1, &uvS2Tex);
	glBindTexture(GL_TEXTURE_2D, uvS2Tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uvS2Tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glm::fvec4 p = findIntersectionPoint(); 
	findIntersection(p);
	std::vector<float> pos{
		p.x,p.y,p.z
	};
	int ind = 0;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &pos[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int), &ind, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	glViewport(0, 0, 512, 512);

	VariableManager vm;
	vm.AddVariable("color", glm::fvec4(1, 1, 1, 1));

	glBindFramebuffer(GL_FRAMEBUFFER, uvS1Fb);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	uvShader.use();
	vm.Apply(uvShader.ID);
	s1UVLine.Render(false, vm);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, uvS2Fb);

	glClearColor(0.2f, 0.2f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	uvShader.use();
	vm.Apply(uvShader.ID);
	s2UVLine.Render(false, vm);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	std::vector<GLbyte> col(512 * 512 * 3, 0);
	glBindTexture(GL_TEXTURE_2D, uvS1Tex); 

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, &col[0]);

	floodFill(col, { 512,512 }, { 255,255,255 }, { 0,0 }, { s1->wrappedU(), s1->wrappedV() });

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 512, 512, GL_RGB, GL_UNSIGNED_BYTE, &col[0]);

	glBindTexture(GL_TEXTURE_2D, uvS2Tex);

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, &col[0]);

	floodFill(col, { 512,512 }, { 255,255,255 }, { 0,0 }, { s2->wrappedU(), s2->wrappedV() });

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 512, 512, GL_RGB, GL_UNSIGNED_BYTE, &col[0]);

	
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, 1600, 900);


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

glm::fvec4 Intersection::findIntersectionPoint()
{
	glm::fvec4 x = { 0.5f,0.5f,0.5f,0.5f };


	int it = 0;
	float val = 1;
	auto prevGrad = distGrad(x);
	auto d = -prevGrad;
	while (fabsf(val)>eps )
	{
		it++;
		float alpha = findAlpha(x, d);
		x  = wrap(x + alpha * d);
		val = distSqr(x);

		auto grad = distGrad(x);
		float beta = glm::dot(grad,grad)/glm::dot(prevGrad,prevGrad);
		d = -grad;// +beta * d;
		prevGrad = grad;

		/*debugVec.push_back(std::make_unique<SceneVector>(
			glm::fvec3(x.x * 10.0f - 5.0f, 0, x.y * 10.0f - 5.0f),
			glm::fvec3(x.x * 10.0f - 5.0f, 0, x.y * 10.0f - 5.0f) + glm::fvec3(grad.x * 10.0f - 5.0f, 0, grad.y * 10.0f - 5.0f),
			glm::fvec4(1, 0, 0, 1)));
		debugVec.push_back(std::make_unique<SceneVector>(
			glm::fvec3(x.z * 10.0f - 5.0f, 0, x.w * 10.0f - 5.0f),
			glm::fvec3(x.z * 10.0f - 5.0f, 0, x.w * 10.0f - 5.0f) + glm::fvec3(grad.z * 10.0f - 5.0f, 0, grad.w * 10.0f - 5.0f),
			glm::fvec4(0, 1, 0, 1)));*/

		//debugVec.push_back(std::make_unique<SceneVector>(
		//	s1->f(x.x, x.y),
		//	s1->f(x.x, x.y) + s1->dfdv(x.x, x.y),
		//	glm::fvec4(0, 1, 0, 1)));
		//debugVec.push_back(std::make_unique<SceneVector>(
		//	s2->f(x.z, x.w),
		//	s2->f(x.z, x.w) + s2->dfdv(x.z, x.w),
		//	glm::fvec4(1, 0, 0, 1)));

		//debugVec.push_back(std::make_unique<SceneVector>(
		//	s1->f(x.x, x.y),
		//	s1->f(x.x, x.y) + glm::fvec3(0, 1, 0),
		//	glm::fvec4(1, 0, 1, 1)));
		//debugVec.push_back(std::make_unique<SceneVector>(
		//	s2->f(x.z, x.w),
		//	s2->f(x.z, x.w) + glm::fvec3(0, 1, 0),
		//	glm::fvec4(1, 1, 0, 1)));
	}
	return x;
}

float Intersection::findAlphaRange(glm::fvec4 x, glm::fvec4 d)
{
	auto maxACond1 = (glm::fvec4(1.0f) - x) / d;
	auto maxACond2 = -x / d;
	std::vector<float> max = {
		maxACond1.x, maxACond1.y, maxACond1.z, maxACond1.w,
		maxACond2.x, maxACond2.y, maxACond2.z, maxACond2.w };
	std::vector<bool> wrapped = {
		s1->wrappedU(), s1->wrappedV(), s2->wrappedU(), s2->wrappedV()
	};

	float maxA = maxAlpha;
	for(int i = 0; i<8; i++)
		if (max[i] > 0 && maxA > max[i] && !wrapped[i%4])
			maxA = max[i];

	float delta = maxA / 100.0f;

	float a2 = delta;
	
	float f1 = distSqr(x), 
		  f2 = distSqr(x + a2 * d);
	if (f2 > f1)
		return a2;
	
	while (true)
	{
		float a3 = a2 + delta;
		if (a3 > maxA)
			return maxA;

		float f3 = distSqr(x + a3 * d);
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

std::vector<glm::fvec4> Intersection::findIntersectionPointsInDirection(glm::fvec4 x, bool dir)
{
	glm::bvec4 wrapped = {
		s1->wrappedU(), s1->wrappedV(), s2->wrappedU(), s2->wrappedV()
	};
	std::vector<glm::fvec4> p;
	glm::fvec4 xPrev;
	glm::fvec3 begin = s1->f(x.x,x.y);
	float dirSgn = dir ? 1 : -1;
	do
	{
		glm::fvec3 dfu = s1->dfdu(x.x, x.y);
		glm::fvec3 dfv = s1->dfdv(x.x, x.y);
		glm::fvec3 dgu = s2->dfdu(x.z, x.w);
		glm::fvec3 dgv = s2->dfdv(x.z, x.w);

		glm::fvec3 n1 = glm::cross(dfu, dfv);
		glm::fvec3 n2 = glm::cross(dgu, dgv);
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
			if (glm::any(glm::lessThan(x, glm::fvec4(eps)) && glm::not_(wrapped)) || 
				glm::any(glm::greaterThan(x, glm::fvec4(1 - eps)) && glm::not_(wrapped)))
				return p;
			x = wrap(x);
			val = { s1->f(x.x,x.y) - s2->f(x.z,x.w), glm::dot(s1->f(x.x,x.y) - s1->f(xPrev.x,xPrev.y),d) - step };
		}
		p.push_back(x);
	} while (glm::length(begin - s1->f(x.x,x.y)) > step / 2.0f);

	return p;
}

void Intersection::findIntersection(glm::fvec4 x)
{
	std::vector<glm::fvec4> uvs;
	std::vector<glm::fvec3> points;
	std::vector<glm::fvec3> s1uvs;
	std::vector<glm::fvec3> s2uvs;

	std::vector<glm::fvec4> p1 = findIntersectionPointsInDirection(x, true);
	std::vector<glm::fvec4> p2 = findIntersectionPointsInDirection(x, false);
	uvs.insert(uvs.end(), p2.rbegin(), p2.rend());
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
	glBindVertexArray(VAO);
	glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);

	for(auto& v : debugVec)
		v->Render(selected, vm);

}

bool Intersection::RenderGui()
{
	ImVec2 size{ 400,400 };
	ImGui::Begin("Intersection");
	ImGui::Checkbox("Enable", &intersect);
	ImGui::Checkbox("Reverse", &reverse);
	ImGui::Image((ImTextureID)uvS1Tex, size);
	ImGui::Image((ImTextureID)uvS2Tex, size);
	ImGui::End();
	return false;
}