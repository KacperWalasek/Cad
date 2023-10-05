#pragma once
#include <string>
#include "nlohmann/json.hpp"
#include "../Simulations/MillingMachine/MillingPath.h"

class FileLoader
{
public:
	static std::string selectFile();
	static std::string selectSaveDest();
	static std::string getLocalPath();
	static void save(std::string path, nlohmann::json json);
	static nlohmann::json load(std::string path);
	static MillingPath loadPath(std::string path);
};