#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/Tools/MeshTool.hpp"

namespace Tool
{
	MeshTool::MeshTool()
	{
	}

	void MeshTool::Draw()
	{
		if (ImGui::Begin("Meshes"))
		{
			auto& modelInstances = World::GetActiveScene()->myModels;

			std::vector<std::string> modelNames;
			for (const auto& model : modelInstances)
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
			Math::Vector3f position = modelInstances[selectedMeshIndex]->GetPosition();
			if (ImGui::DragFloat3("Position", &position.x, 0.1f))
			{
				modelInstances[selectedMeshIndex]->SetPosition(position);
			}

			ImGui::SetNextItemWidth(400);
			Math::Vector3f rotation = modelInstances[selectedMeshIndex]->GetRotation();
			if (ImGui::DragFloat3("Rotation", &rotation.x, 1.0f, 0.0f, 360.0f, "%.3f"))
			{
				modelInstances[selectedMeshIndex]->SetRotation(rotation);
			}

			ImGui::SetNextItemWidth(400);
			Math::Vector3f scale = modelInstances[selectedMeshIndex]->GetScale();
			if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.001f, 100.0f, "%0.3f"))
			{
				modelInstances[selectedMeshIndex]->SetScale(scale);
			}
		}

		ImGui::End();
	}
}