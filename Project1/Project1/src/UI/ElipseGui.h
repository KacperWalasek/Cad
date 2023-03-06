#pragma once
#include "IGui.h"
#include "../Geometry/Elipse.h"

class ElipseGui : public IGui
{
	Elipse& elipse;
public:
	ElipseGui(Elipse& elipse);
	virtual void Render() override;

};