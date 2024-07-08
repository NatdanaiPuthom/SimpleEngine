#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Windows/AssetWindow.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "Engine/Debugger/Console/Console.hpp"

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
				ViewFolders(SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_ASSETS), SIMPLE_DIR_ASSETS);
				ImGui::EndChild();
			}

			ImGui::SameLine();

			parentSize = ImGui::GetContentRegionAvail();

			if (ImGui::BeginChild("Test", parentSize, ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY))
			{
				ImGui::AlignTextToFramePadding();

				if (ImGui::ArrowButton("##Arrow_back", ImGuiDir_Left))
				{
					const size_t lastBackSlashPos = SimpleUtilities::FileManager::myStaticCurrentDirectory.find_last_of('\\');

					if (lastBackSlashPos != std::string::npos)
					{
						const std::string previousDirectory = SimpleUtilities::FileManager::myStaticCurrentDirectory.substr(0, lastBackSlashPos);

						if (previousDirectory.find("Assets") != std::string::npos)
						{
							SimpleUtilities::FileManager::myStaticCurrentDirectory = previousDirectory;
						}
					}
				}

				ImGui::SameLine();
				ImGui::Text(SimpleUtilities::FileManager::myStaticCurrentDirectory.c_str());
				ImGui::Separator();

				DrawFilesInFolder(SimpleUtilities::FileManager::myStaticCurrentDirectory);

				ImGui::EndChild();
			}
		}

		ImGui::End();
	}

	void AssetWindow::Draw()
	{
	}

	void AssetWindow::ViewFolders(const std::string& aStartDirectory, const std::string& aWindowName)
	{
		std::vector<std::string> fileNames = SimpleUtilities::FileManager::GetFileNamesFromDirectory(aStartDirectory, true);

		if (ImGui::TreeNode(aWindowName.c_str()))
		{
			for (auto& name : fileNames)
			{
				if (SimpleUtilities::FileManager::IsFolder(name))
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
				SimpleUtilities::FileManager::myStaticCurrentDirectory = aStartDirectory;
			}
		}
	}

	void AssetWindow::DrawFilesInFolder(const std::string& aDirectory)
	{
		const std::vector<std::string> fileNames = SimpleUtilities::FileManager::GetFileNamesFromDirectory(aDirectory, true);

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
			const std::string extension = SimpleUtilities::FileManager::GetFileExtension(fileNames[i]);

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

			if (ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (extension[0] != '.')
					{
						SimpleUtilities::FileManager::myStaticCurrentDirectory = aDirectory + "\\" + fileNames[i];
						break;
					}
					else if (textureID == sceneIcon)
					{
						const std::string scenePath = "Assets\\Scenes\\" + fileNames[i]; //TO-DO(v11.2.3): Fix so it doesnt become hardcoded
						MainSingleton::GetSceneManager().ChangeScene(scenePath);
					}
				}
				else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && extension[0] == '.')
				{
					myFilePopUpID = "Delete##" + fileNames[i];
					myFileToRemove = aDirectory + "\\" + fileNames[i];
					ImGui::OpenPopup(myFilePopUpID.c_str());
				}
			}

			ImGui::TextWrapped(fileNames[i].c_str());
			ImGui::NextColumn();
		}

		ImGui::Columns();

		ImGui::PopStyleColor();


		if (ImGui::BeginPopup(myFilePopUpID.c_str()))
		{
			if (ImGui::MenuItem("Delete##FileManagerPopUp"))
			{
				if (std::remove(myFileToRemove.c_str()) == 0)
				{
					Simple::Console::Print("Removed ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(myFileToRemove.c_str(), Simple::ConsoleTextColor::Green, true);
				}
			}

			ImGui::EndPopup();
		}
	}
}