#include "ErrorDisplayer.h"

void ErrorDisplayer::AddHandler(std::shared_ptr<IErrorHandler> eh)
{
	errorHandlers.push_back(eh);
}

bool ErrorDisplayer::RenderGui()
{
	ImGui::Begin("Errors");
	for (const auto& eh : errorHandlers)
		for (std::string message : eh->getMessages()) {
			ImGui::Text(message.c_str());
			unsigned int tex = std::stoul(message);
			if (tex == 0)
				continue;
			ImGui::Image((ImTextureID)tex, { 400,400});
		}
	ImGui::End();
	return false;
}
