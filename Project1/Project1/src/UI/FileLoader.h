#pragma once
#include <string>

class FileLoader
{
public:
	static std::string selectFile();
	static std::string getLocalPath();
};