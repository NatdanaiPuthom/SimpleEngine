#pragma once
#include "External/nlohmann/json.hpp"
#include <string>

inline static nlohmann::json ReturnDataAsJSON(float& aValue, const std::string& aVariableName)
{
	nlohmann::json json;
	json[aVariableName] = aValue;
	return json;
}