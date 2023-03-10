#pragma once
#include "IGui.h"
#include "../Geometry/Elipse.h"
#include "../Rendering/AdaptiveRendering.h"

class ElipseGui : public IGui
{
	Elipse& elipse;
	AdaptiveRendering& rendering;
public:
	ElipseGui(Elipse& elipse, AdaptiveRendering& rendering);
	virtual void Render() override;

};