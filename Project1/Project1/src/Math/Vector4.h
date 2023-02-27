#pragma once
#include <vector>
namespace CadMath
{
	class Vector4
	{
	public:
		float values[4];
		bool isFlat = false;
		Vector4();
		Vector4(float rows[4]);
		Vector4(std::vector<float> rows);

		Vector4 transpose();
		Vector4 operator+ (const Vector4& mat) const;
		Vector4 operator- (const Vector4& mat) const;
		Vector4 operator- () const;

		float X() const { return values[0]; }
		float Y() const { return values[1]; }
		float Z() const { return values[2]; }
		float getLength() const;
	};

	Vector4 vectorTo(Vector4 from, Vector4 to);
}
