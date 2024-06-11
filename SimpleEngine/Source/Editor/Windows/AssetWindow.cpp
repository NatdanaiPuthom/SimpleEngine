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
			ImVec2 parentSize = ImGui::GetContentRegionAvail();
			parentSize.x *= 0.2f;

			if (ImGui::BeginChild("AssetPaths#", parentSize, ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX))
			{
				ImGui::SetNextItemOpen(true);
				FileManager::ViewFolders(SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_ASSETS), SIMPLE_DIR_ASSETS);
				ImGui::EndChild();
			}

			ImGui::SameLine();

			parentSize = ImGui::GetContentRegionAvail();

			if (ImGui::BeginChild("Test", parentSize, ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY))
			{
				ImGui::AlignTextToFramePadding();

				if (ImGui::ArrowButton("##Arrow_back", ImGuiDir_Left))
				{
					const size_t lastBackSlashPos = FileManager::myStaticCurrentDirectory.find_last_of('\\');

					if (lastBackSlashPos != std::string::npos)
					{
						const std::string previousDirectory = FileManager::myStaticCurrentDirectory.substr(0, lastBackSlashPos);

						if (previousDirectory.find("Assets") != std::string::npos)
						{
							FileManager::myStaticCurrentDirectory = previousDirectory;
						}
					}
				}

				ImGui::SameLine();
				ImGui::Text(FileManager::myStaticCurrentDirectory.c_str());
				ImGui::Separator();

				FileManager::DrawFilesInFolder(FileManager::myStaticCurrentDirectory);

				ImGui::EndChild();
			}

		}

		ImGui::End();
	}

	void AssetWindow::Draw()
	{
		
	}
}