#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Tools/HierarchyWindow.hpp"

namespace Editor
{
	HierarchyWindow::HierarchyWindow()
	{
	}

	void HierarchyWindow::Init()
	{
	}

	void HierarchyWindow::Update()
	{
	}

	void HierarchyWindow::Draw()
	{
		static int selected = -1;
		if (ImGui::Begin("Hierarchy"))
		{
			Simple::Entities entities = World::GetECS()->GetAllEntities();

			std::string selectedEntityName = "Selected: None";

			if (selected != -1)
			{
				selectedEntityName = "Selected: " + entities[selected]->GetName();
				ImGui::Text(selectedEntityName.c_str());
			}
			else
			{
				ImGui::Text(selectedEntityName.c_str());
			}

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(0.12f, 0.12f, 0.12f, 1.0f).Value);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
			ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

			ImGui::BeginChild("SceneEntities", ImVec2(0.0f, 0.0f), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);

			for (int i = 0; i < entities.GetSize(); ++i)
			{
				if (ImGui::BeginListBox("##Entities"))
				{
					const bool isSelected = (selected == i);

					if (ImGui::Selectable(entities[i]->GetName().c_str(), isSelected))
					{
						selected = i;
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}

					ImGui::EndListBox();
				}
			}

			ImGui::EndChild();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}

		ImGui::End();
	}
}