#pragma once
#include <tuple>

class Scene;
class Camera;
class IClickable
{
public:
	virtual std::tuple<bool, float> InClickRange(Camera& camera, float x, float y) const = 0;
	virtual bool Click(Scene& scene, Camera& camera, float x, float y) = 0;
};