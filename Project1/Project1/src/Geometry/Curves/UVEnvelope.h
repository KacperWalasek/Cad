#pragma once
#include <vector>
#include <glm/glm.hpp>

class UVCurve
{
public:
	UVCurve(std::vector<glm::fvec2> positions)
		: positions(positions)
	{}
	std::vector<glm::fvec2> positions;

};

class SubCurve;
class UVVertex {
public:
	UVVertex(glm::fvec2 uv)
		: uv(uv)
	{}
	glm::fvec2 uv;
	std::vector<SubCurve*> curves;
};

class SubCurve 
{
public:
	SubCurve(UVCurve curve, UVVertex* v1, UVVertex* v2, int i1, int i2)
		: curve(curve), v1(v1), v2(v2), i1(i1), i2(i2)
	{}
	UVVertex *v1, *v2;
	UVCurve curve;
	int i1, i2;
};

class UVEnvelope 
{
public:
	std::vector<SubCurve> curves;
	std::vector<UVVertex> vertices;
};

class UVEnvelopeIterator
{
	UVEnvelope& envelope;
	SubCurve* curve;
	UVVertex* beginVert;
	int i;
	bool finished = false;
	bool reverse = false;
public:
	UVEnvelopeIterator(UVEnvelope& e)
		: envelope(e), curve(&e.curves[0]), beginVert(e.curves[0].v2), i(e.curves[0].i2)
	{}

	glm::fvec2 Next()
	{
		int i1 = (curve->v1 == beginVert && !reverse) ? curve->i1 : curve->i2;
		int i2 = (curve->v1 == beginVert && !reverse) ? curve->i2 : curve->i1;
		if (i2 > i1)
		{
			if (i >= i1 && i < i2)
			{
				i++;
				return Current();
			}
		}
		else
		{
			if (i <= i2 && i>0)
			{
				i--;
				return Current();

			}
			if ((i > i1 && i <= curve->curve.positions.size() - 1))
			{
				i--;
				return Current();
			}
			if (i == 0)
			{
				i = curve->curve.positions.size() - 1;
				return Current();
			}
		}
		beginVert = curve->v1 == beginVert ? curve->v2 : curve->v1;
		curve = beginVert->curves[0] == curve ? beginVert->curves[1] : beginVert->curves[0];
		i = curve->v1 == beginVert ? curve->i1 : curve->i2;
		if (curve == &envelope.curves[0])
		{
			finished = true;
		}
		reverse = true;
		return Current();
	}
	glm::fvec2 Current() const
	{
		return curve->curve.positions[i];
	}

	bool Finished() const { return finished; }
};