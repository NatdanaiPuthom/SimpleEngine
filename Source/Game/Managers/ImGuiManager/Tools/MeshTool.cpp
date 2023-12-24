#include "Game/stdafx.h"
#include "Game/Managers/ImGuiManager/Tools/MeshTool.hpp"

MeshTool::MeshTool() 
{
}

void MeshTool::Draw()
{
	if (ImGui::Begin("Meshes"))
	{
		std::vector<ModelInstance*> modelInstances = SimpleGlobal::GetRenderer()->GetAllModelInstances();
		std::vector<std::string> modelNames;

		for (const ModelInstance* model : modelInstances)
		{
			modelNames.push_back(model->GetName());
		}

		static int selectedMeshIndex = 0;

		std::vector<const char*> names;
		for (const std::string& name : modelNames)
		{
			names.push_back(name.c_str());
		}

		ImGui::SetNextItemWidth(200);
		if (ImGui::Combo("Meshes", &selectedMeshIndex, names.data(), static_cast<int>(names.size())))
		{
		}

		ImGui::Dummy(ImVec2(0, 10));
		ImGui::SetNextItemWidth(400);
		SimpleUtilities::Vector3f position = modelInstances[selectedMeshIndex]->GetPosition();
		if (ImGui::SliderFloat3("Position", &position.x, -10.0f, 20.0f))
		{
			modelInstances[selectedMeshIndex]->SetPosition(position);
		}

		ImGui::SetNextItemWidth(400);
		SimpleUtilities::Vector3f rotation = modelInstances[selectedMeshIndex]->GetRotation();
		if (ImGui::SliderFloat3("Rotation", &rotation.x, 0.0f, 360.0f))
		{
			modelInstances[selectedMeshIndex]->SetRotation(rotation);
		}

		ImGui::SetNextItemWidth(400);
		SimpleUtilities::Vector3f scale = modelInstances[selectedMeshIndex]->GetScale();
		if (ImGui::SliderFloat3("Scale", &scale.x, 1.0f, 20.0f))
		{
			modelInstances[selectedMeshIndex]->SetScale(scale);
		}
	}

	ImGui::End();
}
