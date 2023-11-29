#include "FileLoader.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include <string>
#include <sstream>
#include <windows.h>
#include <fstream>
#include <string>

std::string FileLoader::selectFile()
{
	char* name = tinyfd_openFileDialog("Open file", getLocalPath().c_str(), 0, nullptr, "", true);
	if(name)
		return std::string(name);
	return std::string();
}

std::vector<std::string> FileLoader::selectFiles()
{
	char* n = tinyfd_openFileDialog("Open file", getLocalPath().c_str(), 0, nullptr, "", true);
	if (!n)
		return {};
	std::string namesCombined(n);
	std::string segment;
	std::vector<std::string> names;
	std::stringstream ss(namesCombined);
	while (std::getline(ss, segment, '|'))
	{
		names.push_back(segment);
	}
	return names;
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

MillingPath FileLoader::loadPath(std::string path)
{
	std::string extention = path.substr(path.find_last_of(".") + 1);
	bool flat = extention.at(0) == 'f';
	int diameter = std::stoi(extention.substr(1, extention.size()-1));

	std::ifstream is;
	is.open(path, std::fstream::in);
	std::stringstream buffer;
	buffer << is.rdbuf();
	return MillingPath(buffer, diameter/2.0f, flat);
}

void FileLoader::savePath(std::string filePath, const MillingPath& millingPath)
{
	std::ofstream os;
	os.open(filePath, std::fstream::out | std::fstream::trunc);
	os << millingPath.serialize().rdbuf();
	os.close();
}
