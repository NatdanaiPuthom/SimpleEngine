#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Windows/AssetWindow.hpp"
#include "Editor/FileManager/FileManager.hpp"

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
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(0.18f, 0.18f, 0.18f, 0.80f).Value);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
			ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

			ImVec2 parentSize = ImGui::GetContentRegionAvail();
			parentSize.x *= 0.2f;

			if (ImGui::BeginChild("AssetPaths#", parentSize, ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX))
			{
				FileManager::ViewFolders(SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_ASSETS), SIMPLE_DIR_ASSETS, myCurrentDirectory);
				ImGui::EndChild();
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(0.18f, 0.18f, 0.18f, 0.80f).Value);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
			ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

			parentSize = ImGui::GetContentRegionAvail();

			if (ImGui::BeginChild("Test", parentSize, ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX))
			{
				ImGui::Text(myCurrentDirectory.c_str());
				ImGui::EndChild();
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}
		ImGui::End();
	}
}