#include "Vector4.h"
#include <stdexcept>
#include <math.h>

CadMath::Vector4::Vector4()
	: values{0,0,0,1}
{
}

CadMath::Vector4::Vector4(float vec[4])
{
	for (int i = 0; i < 4; i++)
		values[i] = vec[i];
}

CadMath::Vector4::Vector4(std::vector<float> vec)
{
	if (vec.size() != 4 && vec.size() != 3)
		throw std::invalid_argument("invalid rows number");
	for (int i = 0; i < 3; i++)
		values[i] = vec[i];
	values[3] = vec.size() == 4 ? vec[4] : 1;
}

CadMath::Vector4 CadMath::Vector4::transpose()
{
	isFlat = !isFlat;
	return *this;
}

CadMath::Vector4 CadMath::Vector4::operator+(const Vector4& mat) const
{
	float newValues[4];
	for (int i = 0; i < 4; i++)
		newValues[i] = values[i]+mat.values[i];
	return Vector4(newValues);
}
CadMath::Vector4 CadMath::Vector4::operator-(const Vector4& mat) const
{
	float newValues[4];
	for (int i = 0; i < 4; i++)
		newValues[i] = values[i] - mat.values[i];
	return Vector4(newValues);
}

CadMath::Vector4 CadMath::Vector4::operator-() const
{
	float newValues[4];
	for (int i = 0; i < 4; i++)
		newValues[i] = -values[i];
	return Vector4(newValues);
}

float CadMath::Vector4::getLength() const
{
	return sqrt(powf(X(),2)+powf(Y(),2)+powf(Z(),2));
}

CadMath::Vector4 CadMath::vectorTo(Vector4 from, Vector4 to)
{
	return to - from;
}
