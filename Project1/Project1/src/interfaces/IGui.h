#pragma once
#include "imgui/imgui.h"
#include "ISceneTracker.h"
#include <vector>
#include <memory>

class IGui
{
public:
	virtual bool RenderGui() = 0;
};