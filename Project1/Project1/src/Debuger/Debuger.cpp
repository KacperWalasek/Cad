#include "Debuger.h"

std::vector<unsigned int> Debuger::textures;

void Debuger::ShowTexture(unsigned int tex)
{
	textures.push_back(tex);
}

bool Debuger::RenderGui()
{
	ImGui::Begin("Debuger");
	for (unsigned int tex : textures)
		ImGui::Image(ImTextureID(tex),ImVec2(500, 500));
	ImGui::End();
	return false;
}
