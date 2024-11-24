#include "FlyDataTypes.hpp"
#include "DataType/FlyDataTypeRegistry.hpp"
#include "SystemTypes/FlyWildcard.hpp"
#include "SystemTypes/FlyFlow.hpp"
#include <ImGui/imgui.h>
#include <string>


// Bool
Fly::eIsItemActive ViewAndEdit(bool& aValue)
{
	ImGui::Checkbox("##", &aValue);

	return static_cast<Fly::eIsItemActive>(ImGui::IsItemActive());
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
Fly::eIsItemActive ViewAndEdit(int& aValue)
{
	ImGui::DragInt("##", &aValue);
	return static_cast<Fly::eIsItemActive>(ImGui::IsItemActive());
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
Fly::eIsItemActive ViewAndEdit(float& aValue)
{
	ImGui::DragFloat("##", &aValue);
	return static_cast<Fly::eIsItemActive>(ImGui::IsItemActive());
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
Fly::eIsItemActive ViewAndEdit(uint32& aValue)
{
	int intValue = static_cast<int>(aValue);
	if (ImGui::DragInt("##", &intValue, 1.f, 0, INT_MAX))
	{
		aValue = static_cast<uint32>(intValue);
	}
	return static_cast<Fly::eIsItemActive>(ImGui::IsItemActive());
}

void Save(const uint32& aValue, nlohmann::json& aJson)
{
	aJson["Value"] = aValue;
}

void Load(uint32& aValue, const nlohmann::json& aJson)
{
	aValue = aJson["Value"];
}

// Unsigned Long Long
Fly::eIsItemActive ViewAndEdit(uint64& aValue)
{
	int intValue = static_cast<int>(aValue);
	if (ImGui::DragInt("##", &intValue, 1.f, 0, INT_MAX))
	{
		aValue = static_cast<uint64>(intValue);
	}
	return static_cast<Fly::eIsItemActive>(ImGui::IsItemActive());
}

void Save(const uint64& aValue, nlohmann::json& aJson)
{
	aJson["Value"] = aValue;
}

void Load(uint64& aValue, const nlohmann::json& aJson)
{
	aValue = aJson["Value"];
}

// Char
Fly::eIsItemActive ViewAndEdit(char& aValue)
{
	char c[2] = { aValue, '\0' };

	if (ImGui::InputText("##", c, 2))
	{
		aValue = c[0];
	}
	return static_cast<Fly::eIsItemActive>(ImGui::IsItemActive());
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

	Fly::eIsItemActive ViewAndEdit(std::string& aValue)
	{
		char buffer[32]{};
		strcpy_s(buffer, aValue.c_str());

		if (ImGui::InputText("##", buffer, IM_ARRAYSIZE(buffer)))
		{
			aValue = buffer;
		}
		return static_cast<Fly::eIsItemActive>(ImGui::IsItemActive());
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