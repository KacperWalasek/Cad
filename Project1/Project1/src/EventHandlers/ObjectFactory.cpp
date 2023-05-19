#include "ObjectFactory.h"
#include "../Geometry/Point.h"

ObjectFactory::ObjectFactory(Scene& scene)
	: scene(scene)
{
}

void ObjectFactory::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
		switch (key)
		{
		case GLFW_KEY_P:
			scene.Add(std::make_shared<Point>());
		default:
			break;
		}
}
