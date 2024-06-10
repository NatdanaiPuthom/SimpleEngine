#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/ECSReflection.hpp"
#include "External/imgui.h"

static std::string ExtractStringFromImGuiIDFullName(const std::string& aString)
{
	const size_t position = aString.find("##");

	std::string variableNameWithoutImGuiID;

	if (position != std::string::npos)
	{
		return aString.substr(0, position);
	}

	return aString;
}

bool EditValue(int& aValue, const std::string& aVariableName)
{
	return ImGui::DragInt(aVariableName.c_str(), &aValue);
}

bool EditValue(float& aValue, const std::string& aVariableName)
{
	return ImGui::DragFloat(aVariableName.c_str(), &aValue);
}

bool EditValue(bool& aValue, const std::string& aVariableName)
{
	return ImGui::Checkbox(aVariableName.c_str(), &aValue);
}

bool EditValue(Math::Vector2f& aValue, const std::string& aVariableName)
{
	return ImGui::DragFloat2(aVariableName.c_str(), &aValue.x);
}

bool EditValue(Math::Vector3f& aValue, const std::string& aVariableName)
{
	return ImGui::DragFloat3(aVariableName.c_str(), &aValue.x);
}

bool EditValue(Math::Vector4f& aValue, const std::string& aVariableName)
{
	return ImGui::DragFloat4(aVariableName.c_str(), &aValue.x);
}

bool EditValue(Math::Transform& aValue, const std::string& /*aVariableName*/)
{
	bool edited = false;

	Math::Vector3f position = aValue.GetPosition();
	if (ImGui::DragFloat3("Position##Transform", &position.x, 0.1f))
	{
		edited = true;
		aValue.SetPosition(position);
	}

	Math::Vector3f rotation = aValue.GetRotation();
	if (ImGui::DragFloat3("Rotation##Transform", &rotation.x, 0.01f))
	{
		edited = true;
		aValue.SetRotation(rotation);
	}

	Math::Vector3f scale = aValue.GetScale();
	if (ImGui::DragFloat3("Scale##Transform", &scale.x, 0.01f, 0.001f))
	{
		if (scale.x < 0.001f)
		{
			scale.x = 0.001f;
		}

		if (scale.y < 0.001f)
		{
			scale.y = 0.001f;
		}

		if (scale.z < 0.001f)
		{
			scale.z = 0.001f;
		}

		edited = true;
		aValue.SetScale(scale);
	}

	return edited;
}

bool EditValue(char& aValue, const std::string& aVariableName)
{
	const std::string constCharToString(1, aValue);

	std::string variableNameWithoutImGuiID = ExtractStringFromImGuiIDFullName(aVariableName);
	variableNameWithoutImGuiID += ": " + constCharToString;

	ImGui::Text(variableNameWithoutImGuiID.c_str());

	return true;
}

bool EditValue(const std::string& aValue, const std::string& aVariableName)
{
	std::string variableNameWithoutImGuiID = ExtractStringFromImGuiIDFullName(aVariableName);
	variableNameWithoutImGuiID += ": " + aValue;

	ImGui::Text(variableNameWithoutImGuiID.c_str());

	return true;
}