#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/ImGuiManager/Tools/PlaneReflectionTool.hpp"

Tool::PlaneReflectionTool::PlaneReflectionTool()
{
}

void Tool::PlaneReflectionTool::Draw()
{
	if (ImGui::Begin("Water Reflection"))
	{
		WaterReflectionModel();
	}

	ImGui::End();
}

void Tool::PlaneReflectionTool::WaterReflectionModel()
{
	auto& waterPlane = SimpleWorld::GetWaterPlane()->myModel;

	std::vector<std::string> modelNames;
	modelNames.push_back(waterPlane->GetName());

	std::vector<const char*> names;
	for (const std::string& name : modelNames)
	{
		names.push_back(name.c_str());
	}

	int selectedWaterIndex = 0;
	ImGui::SetNextItemWidth(200);
	if (ImGui::Combo("Plane", &selectedWaterIndex, names.data(), static_cast<int>(names.size())))
	{
	}

	ImGui::Dummy(ImVec2(0, 10));
	ImGui::SetNextItemWidth(400);
	SimpleUtilities::Vector3f position = waterPlane->GetPosition();
	if (ImGui::DragFloat3("Position", &position.x, 0.1f))
	{
		waterPlane->SetPosition(position);
	}

	ImGui::SetNextItemWidth(400);
	SimpleUtilities::Vector3f rotation = waterPlane->GetRotation();
	if (ImGui::DragFloat3("Rotation", &rotation.x, 1.0f, 0.0f, 360.0f, "%.3f"))
	{
		waterPlane->SetRotation(rotation);
	}

	ImGui::SetNextItemWidth(400);
	SimpleUtilities::Vector3f scale = waterPlane->GetScale();
	if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.001f, 100.0f, "%0.3f"))
	{
		waterPlane->SetScale(scale);
	}
}
