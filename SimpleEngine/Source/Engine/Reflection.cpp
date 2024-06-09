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