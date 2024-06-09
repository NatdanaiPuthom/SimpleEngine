#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/Reflection.hpp"
#include "External/imgui.h"

bool EditValue(int& aValue, const std::string& aVariableName)
{
	return ImGui::DragInt(aVariableName.c_str(), &aValue);
}

bool EditValue(bool& aValue, const std::string& aVariableName)
{
	return ImGui::Checkbox(aVariableName.c_str(), &aValue);
}

bool EditValue(Math::Vector3f& aValue, const std::string& aVariableName)
{
	return ImGui::DragFloat3(aVariableName.c_str(), &aValue.x);
}

bool EditValue(Math::Vector4f& aValue, const std::string& aVariableName)
{
	return ImGui::DragFloat4(aVariableName.c_str(), &aValue.x);
}