#include "FlyDataTypes.hpp"
#include "DataType/FlyDataTypeRegistry.hpp"
#include "SystemTypes/FlyWildcard.hpp"
#include "SystemTypes/FlyFlow.hpp"
#include <ImGui/imgui.h>
#include <string>


// Bool
bool Edit(bool& aValue)
{
	bool prev = aValue;
	ImGui::Checkbox("##", &aValue);

	return prev != aValue;
}

void Save(const bool& aValue, nlohmann::json& aJson)
{
	aJson["Value"] = aValue;
}


void Load(bool& aValue, const nlohmann::json& aJson)
{
	aValue = aJson["Value"];
}

// Int
bool Edit(int& aValue)
{
	return ImGui::DragInt("##", &aValue);
}

void Save(const int& aValue, nlohmann::json& aJson)
{
	aJson["Value"] = aValue;
}

void Load(int& aValue, const nlohmann::json& aJson)
{
	aValue = aJson["Value"];
}

// Float
bool Edit(float& aValue)
{
	return ImGui::DragFloat("##", &aValue);
}

void Save(const float& aValue, nlohmann::json& aJson)
{
	aJson["Value"] = aValue;
}

void Load(float& aValue, const nlohmann::json& aJson)
{
	aValue = aJson["Value"];
}

// Unsigned Int
bool Edit(UINT& aValue)
{
	int intValue = static_cast<int>(aValue);
	if (ImGui::DragInt("##", &intValue, 1.f, 0, INT_MAX))
	{
		aValue = static_cast<UINT>(intValue);
		return true;
	}
	return false;
}

void Save(const UINT& aValue, nlohmann::json& aJson)
{
	aJson["Value"] = aValue;
}

void Load(UINT& aValue, const nlohmann::json& aJson)
{
	aValue = aJson["Value"];
}

// Unsigned Long Long
bool Edit(UINT64& aValue)
{
	int intValue = static_cast<int>(aValue);
	if (ImGui::DragInt("##", &intValue, 1.f, 0, INT_MAX))
	{
		aValue = static_cast<UINT64>(intValue);
		return true;
	}
	return false;
}

void Save(const UINT64& aValue, nlohmann::json& aJson)
{
	aJson["Value"] = aValue;
}

void Load(UINT64& aValue, const nlohmann::json& aJson)
{
	aValue = aJson["Value"];
}

// Char
bool Edit(char& aValue)
{
	char c[2] = { aValue, '\0' };

	if (ImGui::InputText("##", c, 2))
	{
		aValue = c[0];
		return true;
	}
	return false;
}

void Save(const char& aValue, nlohmann::json& aJson)
{
	int i = aValue;
	aJson["char"] = i;
}

void Load(char& aValue, const nlohmann::json& aJson)
{
	int i = aJson["char"];
	aValue = (char)i;
}

namespace std
{

	bool Edit(std::string& aValue)
	{
		char buffer[32]{};
		strcpy_s(buffer, aValue.c_str());

		if (ImGui::InputText("##", buffer, IM_ARRAYSIZE(buffer)))
		{
			aValue = buffer;
			return true;
		}
		return false;

	}

	void Save(const std::string& aValue, nlohmann::json& aJson)
	{
		aJson["Value"] = aValue;
	}

	void Load(std::string& aValue, const nlohmann::json& aJson)
	{
		aValue = aJson["Value"];
	}
}