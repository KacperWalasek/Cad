#pragma once
#include <vector>
#include "../interfaces/IGui.h"

class Debuger : public IGui
{
	static std::vector<unsigned int> textures;
public:
	static void ShowTexture(unsigned int tex);

	// Inherited via IGui
	virtual bool RenderGui() override;
};