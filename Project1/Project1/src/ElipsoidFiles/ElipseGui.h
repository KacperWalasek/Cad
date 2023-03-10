#pragma once
#include "../UI/IGui.h"
#include "Elipse.h"
#include "AdaptiveRendering.h"

class ElipseGui : public IGui
{
	Elipse& elipse;
	AdaptiveRendering& rendering;
public:
	ElipseGui(Elipse& elipse, AdaptiveRendering& rendering);
	virtual void Render() override;

};