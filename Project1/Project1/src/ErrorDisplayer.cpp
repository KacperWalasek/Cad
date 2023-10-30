#include "ErrorDisplayer.h"

void ErrorDisplayer::AddHandler(std::shared_ptr<IErrorHandler> eh)
{
	errorHandlers.push_back(eh);
}

bool ErrorDisplayer::RenderGui()
{
	ImGui::Begin("Errors");
	for (const auto& eh : errorHandlers)
		for (std::string message : eh->getMessages()) 
			ImGui::Text(message.c_str());
	ImGui::End();
	return false;
}
