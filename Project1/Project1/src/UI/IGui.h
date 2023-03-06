#pragma once
#include "imgui/imgui.h"

class IGui
{
public:
	virtual void Render() = 0;
};