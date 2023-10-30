#pragma once
#include "interfaces/IErrorHandler.h"
#include "interfaces/IGui.h"
#include <memory>
class ErrorDisplayer : public IGui
{
	std::vector<std::shared_ptr<IErrorHandler>> errorHandlers;
public:

	void AddHandler(std::shared_ptr<IErrorHandler> eh);

	// Inherited via IGui
	virtual bool RenderGui() override;

};