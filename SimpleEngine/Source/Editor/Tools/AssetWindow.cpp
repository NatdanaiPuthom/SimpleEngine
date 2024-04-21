#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Tools/AssetWindow.hpp"

namespace Editor
{
	AssetWindow::AssetWindow()
	{
	}

	void AssetWindow::Init()
	{
	}

	void AssetWindow::Draw()
	{
		if (ImGui::Begin("Assets"))
		{
			/*if (ImGui::BeginChild("child for drag drop"))
			{
								ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImGui::GetStyleColorVec4(ImGuiCol_TitleBg));
				ImGui::BeginChild(ImGui::GetID("assets"), { 0.0f, 0.0f }, false, ImGuiWindowFlags_MenuBar);
				ImGui::BeginGroup();

				if (ImGui::BeginMenuBar())
				{
					ImGui::EndMenuBar();
				}

				ImGui::TableNextColumn();

				ImGui::EndGroup();
				ImGui::EndChild();
				ImGui::PopStyleColor();
			}
				ImGui::EndChild();*/
		}
		ImGui::End();
	}
}