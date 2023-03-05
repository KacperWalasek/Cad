#pragma once
#include <vector>
#include "Vector4.h"
namespace CadMath
{
	class Matrix4x4
	{
	public:
		float values[4][4];
		Matrix4x4();
		Matrix4x4(float rows[4][4]);
		Matrix4x4(std::vector<std::vector<float>> rows);

		Matrix4x4 transpose() const;
		Matrix4x4 operator+ (const Matrix4x4& mat) const;
		Matrix4x4 operator- (const Matrix4x4& mat) const;
		Matrix4x4 operator* (const Matrix4x4& mat) const;
		Vector4 operator* (const Vector4& mat) const;
		Matrix4x4 operator- () const;
		float* operator[] (int i);
		const float* operator[] (int i) const;
	};
}