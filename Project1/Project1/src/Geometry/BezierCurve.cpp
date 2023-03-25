#include "BezierCurve.h"
#include <algorithm>
#include <iterator>
#include "../Math/BernsteinPolynomial.h"

glm::fvec4 BezierCurve::getPointOnCurve(float t) const
{
	std::vector<float> x, y, z;
	std::transform(points.cbegin(), points.cend(), std::back_insert_iterator(x), [](glm::fvec4 p) {return p.x; });
	std::transform(points.cbegin(), points.cend(), std::back_insert_iterator(y), [](glm::fvec4 p) {return p.y; });
	std::transform(points.cbegin(), points.cend(), std::back_insert_iterator(z), [](glm::fvec4 p) {return p.z; });
	BernsteinPolynomial pX(x), pY(y), pZ(z);
	return glm::fvec4(
		pX.Calculate(t),
		pY.Calculate(t),
		pZ.Calculate(t),
		1
	);
}
