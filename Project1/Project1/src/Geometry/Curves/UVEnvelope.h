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
	bool reverseDirection = false;
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
	std::vector<SubCurve*> visited;
	int i;
	bool finished = false;
	bool reverse = false;
	bool wasVisited(SubCurve* c) 
	{
		for (auto visitedCurve : visited)
		{
			if (visitedCurve == c)
				return true;
		}
		return false;
	}
	void nextEnvelope() {
		for (int i = 0; i < envelope.curves.size(); i++)
		{
			if (!wasVisited(&envelope.curves[i]))
			{
				curve = &envelope.curves[i];
				this->i = curve->i1;
				beginVert = curve->v1;
				return;
			}
		}
		finished = true;
	}
public:
	UVEnvelopeIterator(UVEnvelope& e)
		: envelope(e), curve(&e.curves[0]), beginVert(e.curves[0].v1), i(e.curves[0].i1)
	{
		if (e.curves[0].i1 > e.curves[0].i2)
		{
			i = e.curves[0].i2;
			beginVert = e.curves[0].v2;
		}
		else
		{
			i = e.curves[0].i1;
			beginVert = e.curves[0].v1;

		}
		visited.push_back(curve);
	}

	glm::fvec2 Next()
	{
		int i1 = curve->v1 == beginVert ? curve->i1 : curve->i2;
		int i2 = curve->v1 == beginVert ? curve->i2 : curve->i1;
		if (!curve->v1 && !curve->v2 && reverse)
		{
			int tmp = i1;
			i1 = i2;
			i2 = tmp;
		}
		if (i == i2)
		{
			// change curve
			beginVert = curve->v1 == beginVert ? curve->v2 : curve->v1;
			if (beginVert)
			{
				curve = beginVert->curves[0] == curve ? beginVert->curves[1] : beginVert->curves[0];
				i = curve->v1 == beginVert ? curve->i1 : curve->i2;
			}
			if (wasVisited(curve))
				nextEnvelope();

			visited.push_back(curve);
		}
		else
		{
			if (i1 < i2)
			{
				if (!curve->reverseDirection)
					i++;
				else
				{
					if (i == 0)
						i = curve->curve.positions.size() - 1;
					else
						i--;
				}
			}
			else
			{
				if (!curve->reverseDirection)
				{
					i--;
				}
				else
				{
					if (i == curve->curve.positions.size() - 1)
						i = 0;
					else
						i++;
				}
			}
		}
		return Current();
	}
	glm::fvec2 Current() const
	{
		return curve->curve.positions[i];
	}

	bool Finished() const { return finished; }

	bool GoesInPositiveU() const 
	{
		float currentU = Current().x;
		UVEnvelopeIterator copy(*this);
		float nextU = copy.Next().x;
		
		return nextU > currentU && fabsf(nextU - currentU) < 0.5f;
	}

	void Reverse()
	{
		reverse = !reverse;
		beginVert = curve->v1 == beginVert ? curve->v2 : curve->v1;
	}
};