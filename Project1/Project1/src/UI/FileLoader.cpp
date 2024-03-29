#include "FileLoader.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include <string>
#include <sstream>
#include <windows.h>
#include <fstream>

std::string FileLoader::selectFile()
{
	char* name = tinyfd_openFileDialog("Open file", getLocalPath().c_str(), 0, nullptr, "", 0);
	if(name)
		return std::string(name);
	return std::string();
}

std::string FileLoader::selectSaveDest()
{
	char* name = tinyfd_saveFileDialog("Save file", getLocalPath().c_str(), 0, nullptr, "");
	if (name)
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

void FileLoader::save(std::string path, nlohmann::json json)
{
	std::ofstream os;
	os.open(path, std::fstream::out | std::fstream::trunc);
	os << json.dump(1);
	os.close();
}

nlohmann::json FileLoader::load(std::string path)
{
	std::ifstream is;
	is.open(path, std::fstream::in);
	std::stringstream buffer;
	buffer << is.rdbuf();
	
	return nlohmann::json::parse(buffer.str());
}
