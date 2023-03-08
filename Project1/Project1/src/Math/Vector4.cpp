#include "Vector4.h"
#include <stdexcept>
#include <math.h>

CadMath::Vector4::Vector4()
	: values{0,0,0,0}
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
	values[3] = vec.size() == 4 ? vec[3] : 0;
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

CadMath::Vector4 CadMath::Vector4::getColor() const
{
	return ((*this) / getLength() + Vector4({1,1,1,1})) * 255/2;
}

CadMath::Vector4 CadMath::vectorTo(Vector4 from, Vector4 to)
{
	return to - from;
}

CadMath::Vector4 CadMath::operator*(const Vector4& vec, const float& scalar)
{
	return { {vec.X() * scalar,vec.Y() * scalar,vec.Z() * scalar,vec.values[3] * scalar} };
}

CadMath::Vector4 CadMath::operator*(const float& scalar, const Vector4& vec)
{
	return vec * scalar;
}

CadMath::Vector4 CadMath::operator/(const Vector4& vec, const float& scalar)
{
	return { {vec.X() / scalar,vec.Y() / scalar,vec.Z() / scalar,vec.values[3] / scalar} };
}

float CadMath::operator*(const Vector4& vec1, const Vector4& vec2)
{
	return vec1.X()*vec2.X()+
		   vec1.Y()*vec2.Y() + 
		   vec1.Z()*vec2.Z() + 
		   vec1.values[3]*vec2.values[3];
}
