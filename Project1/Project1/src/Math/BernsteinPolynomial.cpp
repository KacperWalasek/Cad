#include "BernsteinPolynomial.h"

BernsteinPolynomial::BernsteinPolynomial(std::vector<float> b)
	:b(b)
{
}

float BernsteinPolynomial::Calculate(float t)
{
	std::vector<float> level = b;
	while (level.size() > 1)
	{
		std::vector<float> nextLevel;
		for (int i = 0; i < level.size() - 1; i++)
			nextLevel.push_back(level[i] * (1 - t) + level[i + 1] * t);
		level = nextLevel;
	}

	return level[0];
}
