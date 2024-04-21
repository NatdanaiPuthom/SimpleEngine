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
			const auto& entities = World::GetECS()->GetAllEntities();

			for (int i = 0; i < entities.size(); ++i)
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
				}
				ImGui::EndListBox();
			}		
		}

		ImGui::End();
	}
}