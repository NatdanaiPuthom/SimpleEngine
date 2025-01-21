#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MainMenuTabs/SceneTab.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneTab::SceneTab(const std::string& aName)
		: MainMenuTab(aName)
	{
	}

	void SceneTab::OnClick()
	{

		if (ImGui::BeginMenu("Load##SceneMenuItem"))
		{
			ShowSceneList();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Create##SceneMenuItem"))
		{
			CreateNewScene();
			ImGui::EndMenu();
		}
	}

	void SceneTab::CreateNewScene()
	{
		const static std::vector<std::string> menuNames({ "New scene", "Copy scene" });
		Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();

		for (size_t i = 0; i < menuNames.size(); i++)
		{
			if (ImGui::Selectable(menuNames[i].c_str()))
			{
				switch (i)
				{
				case 0:
				{
					const std::string absolutePath = SimpleUtilities::GetAbsolutePath(SimpleUtilities::AppendCounterIfAlreadyExist(std::string(SIMPLE_DIR_SCENES) + "\\" + std::string(SIMPLE_FILENAME_NEWSCENE)));
					const std::string relativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(absolutePath);
					sceneManager.CreateNewScene(absolutePath);
					sceneManager.ChangeScene(relativePath);
					break;
				}
				case 1:
				{
					const std::string absolutePath = sceneManager.GetCurrentSceneInfo()->absolutePath;
					const std::string newCopyName = SimpleUtilities::AppendStringBeforeDot("_Copy", absolutePath);
					const std::string newFileName = SimpleUtilities::AppendCounterIfAlreadyExist(newCopyName);
					const std::string relativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(newFileName);

					std::filesystem::copy_file(absolutePath, newFileName, std::filesystem::copy_options::overwrite_existing);
					sceneManager.ChangeScene(relativePath);
					break;
				}
				default:
					break;
				}
				break;
			}

			if (i == 1) //NOTE(v11.4.2): Shouldn't be hardcoded value, whole Editor project need refactor someday
			{
				//static float createNewSceneAsCopyTooltipDelayTimer = 0.0f;
				//ShowHoveredToolTips("Create new copy of this scene", createNewSceneAsCopyTooltipDelayTimer);
			}
		}
	}

	void SceneTab::ShowSceneList()
	{
		const std::vector<std::string> sceneNames = SimpleUtilities::FileManager::GetFileNamesFromDirectory(SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_SCENES));

		for (const auto& name : sceneNames)
		{
			if (ImGui::Selectable(name.c_str()))
			{
				const std::string scenePath = std::string(SIMPLE_DIR_SCENES) + "\\" + name;
				MainSingleton::GetSceneManager().ChangeScene(scenePath);
				break;
			}
		}
	}
}
