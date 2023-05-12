#pragma once
#include "imgui/imgui.h"
#include "ISceneTracker.h"
#include <vector>
#include <memory>

class IGui
{
public:
	virtual bool RenderGui(std::vector<std::shared_ptr<ISceneTracker>>& trackers) = 0;
};