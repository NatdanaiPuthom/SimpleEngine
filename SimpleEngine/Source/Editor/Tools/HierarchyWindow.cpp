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

		if (ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_None))
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(0.12f, 0.12f, 0.12f, 1.0f).Value);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
			ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

			if (ImGui::BeginChild("SceneEntities", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Border))
			{
				Simple::Entities entities = World::GetECS()->GetAllEntities();
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
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}
		ImGui::End();

		if (ImGui::Begin("Inspector"))
		{
			if (selected != -1)
			{
			}
			else
			{
			}

			std::string searchComponent = "";
			if (ImGui::InputTextWithHint("Search", "Example \"TransformComponent\"", &searchComponent[0], searchComponent.capacity() + 1))
			{
			}

			static bool isListOpen = false;
			if (ImGui::Button("Add Component"))
			{
				isListOpen = !isListOpen;
			}

			if (isListOpen == true)
			{
				std::array<std::string, 2> components;
				components[0] = "Transform";
				components[1] = "Mesh";

				for (size_t i = 0; i < components.size(); ++i)
				{
					if (ImGui::BeginListBox("##ComponentList"))
					{
						const bool isSelected = false;

						if (ImGui::Selectable(components[i].c_str(), isSelected))
						{
							
						}

						ImGui::EndListBox();
					}
				}

				

				if (!ImGui::IsItemHovered() && ImGui::IsMouseDown(0))
				{
					isListOpen = false;
				}
			}
		}

		ImGui::End();
	}
}