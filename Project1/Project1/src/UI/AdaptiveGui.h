#pragma once
#include "IGui.h"
#include "../Rendering/AdaptiveRendering.h"

class AdaptiveGui : public IGui
{
	AdaptiveRendering& rendering;
public:
	AdaptiveGui(AdaptiveRendering& rendering);
	virtual void Render() override;

};