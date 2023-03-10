#pragma once
#include "../UI/IGui.h"
#include "AdaptiveRendering.h"

class AdaptiveGui : public IGui
{
	AdaptiveRendering& rendering;
public:
	AdaptiveGui(AdaptiveRendering& rendering);
	virtual void Render() override;

};