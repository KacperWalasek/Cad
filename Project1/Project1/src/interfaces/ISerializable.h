#pragma once
#include <nlohmann/json.hpp>
#include "../Scene/Scene.h"
#include "../Indexer.h"

class ISerializable
{
public:
	virtual nlohmann::json Serialize(Scene& scene, Indexer& indexer, std::map<int, int>& pointIndexMap) const = 0;
};