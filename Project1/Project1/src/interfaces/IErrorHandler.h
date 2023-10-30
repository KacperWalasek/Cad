#pragma once
#include <string>
#include <vector>

class IErrorHandler 
{
public:
	virtual std::vector<std::string> getMessages() const = 0;
};