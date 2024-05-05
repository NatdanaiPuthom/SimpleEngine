#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/FileManager/FileManager.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"

namespace Editor
{
	void FileManager::DropFiles(HDROP aHDROP)
	{
		char filePath[MAX_PATH]{};

		const UINT count = DragQueryFileA(aHDROP, 0, NULL, 0);

		for (UINT i = 0; i < count; ++i)
		{
			if (DragQueryFileA(aHDROP, i, filePath, MAX_PATH))
			{
				const std::string extension = GetFileExtension(filePath);
				const std::string name = GetFileName(filePath);

				UNREFERENCED_PARAMETER(name);

				if (extension == ".json")
				{
					std::cout << "File type: json" << std::endl;
				}
				else if (extension == ".fbx")
				{
					std::cout << "File type: fbx" << std::endl;
				}
			}
		}
	}

	bool FileManager::IsFolder(const std::string& aFileName)
	{
		return (std::string::npos == aFileName.find_last_of('.'));
	}

	void FileManager::ViewFolders(const std::string& aStartDirectory, const std::string& aWindowName, std::string& aCurrentDirectory)
	{
		std::vector<std::string> fileNames = FileManager::GetFileNamesFromDirectory(aStartDirectory, true);

		if (ImGui::TreeNode(aWindowName.c_str()))
		{
			for (auto& name : fileNames)
			{
				if (FileManager::IsFolder(name))
				{
					ViewFolders(aStartDirectory + "\\" + name, name, aCurrentDirectory);
				}
				else
				{
					if (ImGui::Selectable(name.c_str()))
					{

					}
				}
			}

			ImGui::TreePop();
		}
		else
		{
			if (ImGui::IsItemClicked())
			{
				aCurrentDirectory = aStartDirectory;
			}
		}
	}

	void FileManager::DrawFilesInFolder(const std::string& aDirectory)
	{
		const std::vector<std::string> fileNames = FileManager::GetFileNamesFromDirectory(aDirectory, true);

		ID3D11ShaderResourceView* textureCat = Global::GetGraphicsEngine()->GetTexture("Cat.dds")->GetShaderResourceView().Get();
		ID3D11ShaderResourceView* textureHamster = Global::GetGraphicsEngine()->GetTexture("Hamster.dds")->GetShaderResourceView().Get();
		ID3D11ShaderResourceView* textureScaredCat = Global::GetGraphicsEngine()->GetTexture("Cat-scared.dds")->GetShaderResourceView().Get();
		ID3D11ShaderResourceView* textureDefault = Global::GetGraphicsEngine()->GetTexture("DefaultTexture.dds")->GetShaderResourceView().Get();

		const ImVec2 windowSize = ImGui::GetContentRegionAvail();
		const size_t iconSize = 100;

		size_t x = 0;

		ImGui::PushStyleColor(ImGuiCol_Button, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(1.0f, 0.0f, 1.0f, 1.0f).Value);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor(1.0f, 0.8f, 0.0f, 1.0f).Value);

		for (size_t i = 0; i < fileNames.size(); ++i)
		{
			const std::string extension = GetFileExtension(fileNames[i]);

			ImTextureID textureID;

			if (extension == ".json")
			{
				textureID = textureCat;
			}
			else if (extension == ".fbx")
			{
				textureID = textureHamster;
			}
			else if (extension == ".dds")
			{
				textureID = Global::GetGraphicsEngine()->GetTexture(fileNames[i].c_str())->GetShaderResourceView().Get();
			}
			else if (extension.find('.') == std::string::npos)
			{
				textureID = textureScaredCat;
			}
			else
			{
				textureID = textureDefault;
			}

			ImGui::ImageButton(textureID, ImVec2(iconSize, iconSize));

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip(fileNames[i].c_str());
			}

			x += iconSize;

			if (x <= windowSize.x - iconSize)
			{
				ImGui::SameLine();
			}
			else
			{
				x = 0;
			}
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	const std::string FileManager::GetFileExtension(const std::string& aFilePath)
	{
		for (size_t i = aFilePath.length(); i > 0; i--)
		{
			if (aFilePath[i] == '.')
			{
				return aFilePath.substr(i, aFilePath.length());
			}

			if (aFilePath[i] == '\\')
			{
				break;
			}
		}

		return aFilePath;
	}

	const std::string FileManager::GetFileName(const std::string& aFilePath)
	{
		for (size_t i = aFilePath.length(); i > 0; i--)
		{
			if (aFilePath[i] == '\\')
			{
				return aFilePath.substr(i + 1, aFilePath.length());
			}
		}

		return aFilePath;
	}

	std::vector<std::string> FileManager::GetFileNamesFromDirectory(const std::string& aPath, const bool aIncludeFolders)
	{
		std::vector<std::string> fileNames;

		for (const auto& entry : std::filesystem::directory_iterator(aPath))
		{
			if (!aIncludeFolders && !entry.is_regular_file())
			{
				continue;
			}

			fileNames.push_back(entry.path().filename().string());
		}

		return fileNames;
	}
}