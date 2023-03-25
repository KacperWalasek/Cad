#pragma once
#include <vector>
class BernsteinPolynomial
{
	std::vector<float> b;
public:
	BernsteinPolynomial(std::vector<float> b);
	float Calculate(float t);
};