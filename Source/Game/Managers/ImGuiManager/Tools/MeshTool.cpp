#include "Game/stdafx.h"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Game/Managers/ImGuiManager/Tools/MeshTool.hpp"

MeshTool::MeshTool(Renderer* aRenderer) 
	: myRenderer(aRenderer)
{
}

void MeshTool::Draw()
{
	if (ImGui::Begin("Meshes"))
	{
		std::vector<Mesh*> meshes = myRenderer->GetMeshes();
		std::vector<std::string> meshNames;

		for (const Mesh* mesh : meshes)
		{
			meshNames.push_back(mesh->GetName());
		}

		static int selectedMeshIndex = 0;

		std::vector<const char*> names;
		for (const std::string& name : meshNames)
		{
			names.push_back(name.c_str());
		}

		ImGui::SetNextItemWidth(200);
		if (ImGui::Combo("Meshes", &selectedMeshIndex, names.data(), static_cast<int>(names.size())))
		{
		}

		ImGui::Dummy(ImVec2(0, 10));
		ImGui::SetNextItemWidth(400);
		SimpleUtilities::Vector3f position = meshes[selectedMeshIndex]->GetPosition();
		if (ImGui::SliderFloat3("Position", &position.x, -10.0f, 20.0f))
		{
			meshes[selectedMeshIndex]->SetPosition(position);
		}

		ImGui::SetNextItemWidth(400);
		SimpleUtilities::Vector3f rotation = meshes[selectedMeshIndex]->GetRotation();
		if (ImGui::SliderFloat3("Rotation", &rotation.x, 0.0f, 360.0f))
		{
			meshes[selectedMeshIndex]->SetRotation(rotation);
		}

		ImGui::SetNextItemWidth(400);
		SimpleUtilities::Vector3f scale = meshes[selectedMeshIndex]->GetScale();
		if (ImGui::SliderFloat3("Scale", &scale.x, 1.0f, 20.0f))
		{
			meshes[selectedMeshIndex]->SetScale(scale);
		}
	}

	ImGui::End();
}
