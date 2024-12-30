#include "Engine/Precomplied/EnginePch.hpp"
#include "NodeScriptDataTypes.hpp"
#include <string>


// Bool
Fly::ViewAndEditResult ViewAndEdit(bool& aValue)
{
	ImGui::Checkbox("##", &aValue);
	Fly::ViewAndEditResult result;
	result.mIsItemActive = ImGui::IsItemActive();
	return result;
}

void View(const bool& aValue)
{
	bool v = aValue;
	ImGui::BeginDisabled();
	ImGui::Checkbox("##", &v);
	ImGui::EndDisabled();
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
Fly::ViewAndEditResult ViewAndEdit(int& aValue)
{
	ImGui::DragInt("##", &aValue);
	Fly::ViewAndEditResult result;
	result.mIsItemActive = ImGui::IsItemActive();
	return result;
}

void View(const int& aValue)
{
	int v = aValue;
	ImGui::BeginDisabled();
	ImGui::DragInt("##", &v);
	ImGui::EndDisabled();
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
Fly::ViewAndEditResult ViewAndEdit(float& aValue)
{
	ImGui::DragFloat("##", &aValue);
	Fly::ViewAndEditResult result;
	result.mIsItemActive = ImGui::IsItemActive();
	return result;
}

void View(const float& aValue)
{
	float v = aValue;
	ImGui::BeginDisabled();
	ImGui::DragFloat("##", &v);
	ImGui::EndDisabled();
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
Fly::ViewAndEditResult ViewAndEdit(uint32& aValue)
{
	int intValue = static_cast<int>(aValue);
	if (ImGui::DragInt("##", &intValue, 1.f, 0, INT_MAX))
	{
		aValue = static_cast<uint32>(intValue);
	}
	Fly::ViewAndEditResult result;
	result.mIsItemActive = ImGui::IsItemActive();
	return result;
}

void View(const uint32& aValue)
{
	int intValue = static_cast<int>(aValue);
	ImGui::BeginDisabled();
	ImGui::DragInt("##", &intValue, 1.f, 0, INT_MAX);
	ImGui::EndDisabled();
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
Fly::ViewAndEditResult ViewAndEdit(uint64& aValue)
{
	int intValue = static_cast<int>(aValue);
	if (ImGui::DragInt("##", &intValue, 1.f, 0, INT_MAX))
	{
		aValue = static_cast<uint64>(intValue);
	}

	Fly::ViewAndEditResult result;
	result.mIsItemActive = ImGui::IsItemActive();
	return result;
}

void View(const uint64& aValue)
{
	int intValue = static_cast<int>(aValue);
	ImGui::BeginDisabled();
	ImGui::DragInt("##", &intValue, 1.f, 0, INT_MAX);
	ImGui::EndDisabled();
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
Fly::ViewAndEditResult ViewAndEdit(char& aValue)
{
	char c[2] = { aValue, '\0' };

	if (ImGui::InputText("##", c, 2))
	{
		aValue = c[0];
	}
	Fly::ViewAndEditResult result;
	result.mIsItemActive = ImGui::IsItemActive();
	return result;
}

void View(const char& aValue)
{
	char c[2] = { aValue, '\0' };
	ImGui::BeginDisabled();
	ImGui::InputText("##", c, 2);
	ImGui::EndDisabled();
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

	Fly::ViewAndEditResult ViewAndEdit(std::string& aValue)
	{
		char buffer[32]{};
		strcpy_s(buffer, aValue.c_str());

		if (ImGui::InputText("##", buffer, IM_ARRAYSIZE(buffer)))
		{
			aValue = buffer;
		}
		Fly::ViewAndEditResult result;
		result.mIsItemActive = ImGui::IsItemActive();
		return result;
	}

	void View(const std::string& aValue)
	{
		char buffer[32]{};
		strcpy_s(buffer, aValue.c_str());

		ImGui::BeginDisabled();
		ImGui::InputText("##", buffer, IM_ARRAYSIZE(buffer));
		ImGui::EndDisabled();
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