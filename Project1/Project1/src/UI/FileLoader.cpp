#include "FileLoader.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include <string>
#include <windows.h>

std::string FileLoader::selectFile()
{
	char* name = tinyfd_openFileDialog("Open file", getLocalPath().c_str(), 0, nullptr, "", 0);
	if(name)
		return std::string(name);
	return std::string();
}

std::string FileLoader::getLocalPath()
{
	char pBuf[256];
	size_t len = sizeof(pBuf);

	GetModuleFileName(NULL, pBuf, len);
	std::string::size_type pos = std::string(pBuf).find_last_of("\\/");

	return std::string(pBuf).substr(0, pos) + "\\..\\..\\..\\..\\";
}
