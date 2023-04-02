#pragma once
#include <string>

class ISceneElement
{
public:
	virtual std::string getName() const = 0;
};