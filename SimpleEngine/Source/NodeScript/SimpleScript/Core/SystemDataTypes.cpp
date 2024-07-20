#include "SystemDataTypes.hpp"
#include "DataType/DataTypeRegistry.hpp"
#include "SystemTypes/ScriptWildcard.hpp"
#include "SystemTypes/ScriptFlow.hpp"
#include <ImGui/imgui.h>
#include <string>


// Bool
bool Edit(bool& aValue)
{
	bool prev = aValue;
	ImGui::Checkbox("##", &aValue);

	return prev != aValue;
}

void Save(nlohmann::json& aJson, const bool& aValue)
{
	aJson["Value"] = aValue;
}


void Load(const nlohmann::json& aJson, bool& aValue)
{
	aValue = aJson["Value"];
}

// Int
bool Edit(int& aValue)
{
	return ImGui::DragInt("##", &aValue);
}

void Save(nlohmann::json& aJson, const int& aValue)
{
	aJson["Value"] = aValue;
}

void Load(const nlohmann::json& aJson, int& aValue)
{
	aValue = aJson["Value"];
}

// Float
bool Edit(float& aValue)
{
	return ImGui::DragFloat("##", &aValue);
}

void Save(nlohmann::json& aJson, const float& aValue)
{
	aJson["Value"] = aValue;
}

void Load(const nlohmann::json& aJson, float& aValue)
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

void Save(nlohmann::json& aJson, const UINT& aValue)
{
	aJson["Value"] = aValue;
}

void Load(const nlohmann::json& aJson, UINT& aValue)
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

void Save(nlohmann::json& aJson, const UINT64& aValue)
{
	aJson["Value"] = aValue;
}

void Load(const nlohmann::json& aJson, UINT64& aValue)
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

void Save(nlohmann::json& aJson, const char& aValue)
{
	int i = aValue;
	aJson["char"] = i;
}

void Load(const nlohmann::json& aJson, char& aValue)
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

	void Save(nlohmann::json& aJson, const std::string& aValue)
	{
		aJson["Value"] = aValue;
	}

	void Load(const nlohmann::json& aJson, std::string& aValue)
	{
		aValue = aJson["Value"];
	}
}


namespace FLY_NAMESPACE
{

	

	void Test123()
	{
	}
}
