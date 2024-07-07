#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/FileManager/FileManager.hpp"
#include "Graphics/Defines.hpp"
#include "External/imgui.h"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	bool FileManager::myStaticHasDragDropBegin = false;
	std::string FileManager::myStaticCurrentDirectory = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_ASSETS);

	void FileManager::DropFiles(HDROP aHDROP)
	{
		char filePath[MAX_PATH]{};

		const UINT count = DragQueryFileA(aHDROP, 0, NULL, 0);

		for (UINT i = 0; i < count; ++i)
		{
			if (DragQueryFileA(aHDROP, i, filePath, MAX_PATH))
			{
				const std::string name = GetFileName(filePath);
				const std::string destinationPath = myStaticCurrentDirectory + "\\" + name;

				if (CopyFileA(filePath, destinationPath.c_str(), FALSE))
				{
					HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
					CONSOLE_SCREEN_BUFFER_INFO defaultInfo;
					GetConsoleScreenBufferInfo(hConsole, &defaultInfo);
					WORD defaultAttributes = defaultInfo.wAttributes;

					std::cout << "File: ";

					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					std::cout << name;

					SetConsoleTextAttribute(hConsole, defaultAttributes);
					std::cout << " has been copied to ";

					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					std::cout << myStaticCurrentDirectory << std::endl;

					SetConsoleTextAttribute(hConsole, defaultAttributes);
				}
			}
		}
	}

	bool FileManager::IsFolder(const std::string& aFileName)
	{
		return (std::string::npos == aFileName.find_last_of('.'));
	}

	void FileManager::ViewFolders(const std::string& aStartDirectory, const std::string& aWindowName)
	{
		std::vector<std::string> fileNames = FileManager::GetFileNamesFromDirectory(aStartDirectory, true);

		if (ImGui::TreeNode(aWindowName.c_str()))
		{
			for (auto& name : fileNames)
			{
				if (FileManager::IsFolder(name))
				{
					ViewFolders(aStartDirectory + "\\" + name, name);
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
				myStaticCurrentDirectory = aStartDirectory;
			}
		}
	}

	void FileManager::DrawFilesInFolder(const std::string& aDirectory)
	{
		const std::vector<std::string> fileNames = FileManager::GetFileNamesFromDirectory(aDirectory, true);

		ID3D11ShaderResourceView* unknownIcon = Global::GetGraphicsEngine()->GetIcon(Graphics::eIconType::Unknown)->GetShaderResourceView().Get();

		ID3D11ShaderResourceView* cubeMapIcon = Global::GetGraphicsEngine()->GetIcon(Graphics::eIconType::CubeMap)->GetShaderResourceView().Get();
		ID3D11ShaderResourceView* folderIcon = Global::GetGraphicsEngine()->GetIcon(Graphics::eIconType::Folder)->GetShaderResourceView().Get();
		ID3D11ShaderResourceView* cursorIcon = Global::GetGraphicsEngine()->GetIcon(Graphics::eIconType::Cursor)->GetShaderResourceView().Get();
		ID3D11ShaderResourceView* sceneIcon = Global::GetGraphicsEngine()->GetIcon(Graphics::eIconType::Scene)->GetShaderResourceView().Get();
		ID3D11ShaderResourceView* fbxIcon = Global::GetGraphicsEngine()->GetIcon(Graphics::eIconType::FBX)->GetShaderResourceView().Get();
		ID3D11ShaderResourceView* pngIcon = Global::GetGraphicsEngine()->GetIcon(Graphics::eIconType::PNG)->GetShaderResourceView().Get();
		ID3D11ShaderResourceView* jpgIcon = Global::GetGraphicsEngine()->GetIcon(Graphics::eIconType::JPG)->GetShaderResourceView().Get();
		ID3D11ShaderResourceView* objIcon = Global::GetGraphicsEngine()->GetIcon(Graphics::eIconType::OBJ)->GetShaderResourceView().Get();
		ID3D11ShaderResourceView* mp3Icon = Global::GetGraphicsEngine()->GetIcon(Graphics::eIconType::MP3)->GetShaderResourceView().Get();

		static constexpr float padding = 16.0f;
		static constexpr float thumbnailSize = 64.0f;
		static constexpr float cellSize = thumbnailSize + padding;
		const float panelWidth = ImGui::GetContentRegionAvail().x;

		int columnCount = (int)(panelWidth / cellSize);

		if (columnCount < 1)
		{
			columnCount = 1;
		}

		ImGui::Columns(columnCount, 0, false);

		ImGui::PushStyleColor(ImGuiCol_Button, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);

		for (size_t i = 0; i < fileNames.size(); ++i)
		{
			const std::string extension = GetFileExtension(fileNames[i]);

			ImTextureID textureID = unknownIcon;

			if (extension[0] != '.')
			{
				textureID = folderIcon;
			}
			else if (extension == ".dds")
			{
				const std::string texturePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(aDirectory) + "\\" + fileNames[i];
				const std::shared_ptr<const Graphics::Texture> texture = Global::GetGraphicsEngine()->GetTexture(texturePath.c_str());

				if (texture->GetSlot() != Graphics::Global_Slot_CubeMap)
				{
					textureID = texture->GetShaderResourceView().Get();
				}
				else
				{
					textureID = cubeMapIcon;
				}

			}
			else if (extension == ".fbx")
			{
				textureID = fbxIcon;
			}
			else if (extension == ".scene")
			{
				textureID = sceneIcon;
			}
			else if (extension == ".png")
			{
				textureID = pngIcon;
			}
			else if (extension == ".jpg")
			{
				textureID = jpgIcon;
			}
			else if (extension == ".obj")
			{
				textureID = objIcon;
			}
			else if (extension == ".mp3")
			{
				textureID = mp3Icon;
			}
			else if (extension == ".cur")
			{
				textureID = cursorIcon;
			}

			ImGui::ImageButton(fileNames[i].c_str(), textureID, { thumbnailSize, thumbnailSize });

			if (ImGui::BeginDragDropSource())
			{
				if (extension[0] == '.')
				{
					std::string filePath = aDirectory + "\\" + fileNames[i];

					char buffer[256];
					strcpy_s(buffer, filePath.c_str());

					ImGui::SetDragDropPayload("Assets_Browser", buffer, sizeof(buffer));

					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

					ImGui::BeginTooltip();
					ImGui::ImageButton(textureID, ImVec2(64.0f, 64.0f));
					ImGui::EndTooltip();

					ImGui::PopStyleVar();
				}

				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (extension[0] != '.')
				{
					myStaticCurrentDirectory = aDirectory + "\\" + fileNames[i];
					break;
				}
				else if (textureID == sceneIcon)
				{
					const std::string scenePath = "Assets\\Scenes\\" + fileNames[i]; //TO-DO(v11.2.3): Fix so it doesnt become hardcoded
					MainSingleton::GetSceneManager().ChangeScene(scenePath);
				}
			}

			ImGui::TextWrapped(fileNames[i].c_str());
			ImGui::NextColumn();
		}

		ImGui::Columns();

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