#include "Matrix4x4.h"
#include <stdexcept>

CadMath::Matrix4x4::Matrix4x4()
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			values[i][j] = i==j ? 1 : 0;
}

CadMath::Matrix4x4::Matrix4x4(float rows[4][4])
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			values[i][j] = rows[i][j];
}

CadMath::Matrix4x4::Matrix4x4(std::vector<std::vector<float>> rows)
{
	if (rows.size() != 4)
		throw std::invalid_argument("invalid rows number");
	for (int i = 0; i < 4; i++)
	{
		if(rows[i].size()!=4)
			throw std::invalid_argument("invalid row size");

		for (int j = 0; j < 4; j++)
			values[i][j] = rows[i][j];
	}
}

CadMath::Matrix4x4 CadMath::Matrix4x4::transpose() const
{
	float newNumbers[4][4];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			newNumbers[i][j] = values[j][i];

	return Matrix4x4(newNumbers);
}

CadMath::Matrix4x4 CadMath::Matrix4x4::operator+(const Matrix4x4& mat) const
{
	float newNumbers[4][4];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			newNumbers[i][j] = values[i][j] + mat.values[i][j];

	return Matrix4x4(newNumbers);
}

CadMath::Matrix4x4 CadMath::Matrix4x4::operator-(const Matrix4x4& mat) const
{
	float newNumbers[4][4];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			newNumbers[i][j] = values[i][j] - mat.values[i][j];

	return Matrix4x4(newNumbers);
}

CadMath::Matrix4x4 CadMath::Matrix4x4::operator*(const Matrix4x4& mat) const
{
	float newNumbers[4][4];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			newNumbers[i][j] = 0;
			for (int k = 0; k < 4; k++)
				newNumbers[i][j] += values[i][k] * mat.values[k][j];
		}

	return Matrix4x4(newNumbers);
}

CadMath::Vector4 CadMath::Matrix4x4::operator*(const Vector4& vec) const
{
	if (vec.isFlat)
		throw std::invalid_argument("vector is flat");
	float newNumbers[4];
	for (int i = 0; i < 4; i++)
	{
		newNumbers[i] = 0;
		for (int k = 0; k < 4; k++)
			newNumbers[i] += values[i][k] * vec.values[k];
	}
	return Vector4(newNumbers);
}

CadMath::Matrix4x4 CadMath::Matrix4x4::operator-() const
{
	float newNumbers[4][4];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			newNumbers[i][j] = -values[i][j];
	return Matrix4x4(newNumbers);
}

float* CadMath::Matrix4x4::operator[](int i)
{
	return values[i];
}

const float* CadMath::Matrix4x4::operator[](int i) const
{
	return values[i];
}
