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

	void AssetWindow::Update()
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
				ImGui::SetNextItemOpen(true);
				FileManager::ViewFolders(SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_ASSETS), SIMPLE_DIR_ASSETS);
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

			if (ImGui::BeginChild("Test", parentSize, ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY))
			{
				ImGui::AlignTextToFramePadding();

				if (ImGui::ArrowButton("##Arrow_back", ImGuiDir_Left))
				{
					const size_t lastBackSlashPos = FileManager::sCurrentDirectory.find_last_of('\\');

					if (lastBackSlashPos != std::string::npos)
					{
						const std::string previousDirectory = FileManager::sCurrentDirectory.substr(0, lastBackSlashPos);

						if (previousDirectory.find("Assets") != std::string::npos)
						{
							FileManager::sCurrentDirectory = previousDirectory;
						}
					}
				}

				ImGui::SameLine();
				ImGui::Text(FileManager::sCurrentDirectory.c_str());
				ImGui::Separator();

				FileManager::DrawFilesInFolder(FileManager::sCurrentDirectory);

				ImGui::EndChild();
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}

		ImGui::End();
	}

	void AssetWindow::Draw()
	{
		
	}
}