#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/SceneMenuBar.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneMenuBar::SceneMenuBar()
	{
	}

	SceneMenuBar::~SceneMenuBar()
	{
	}

	void SceneMenuBar::Init()
	{
	}

	void SceneMenuBar::Update()
	{
		if (MainSingleton::GetInputManager().IsKeyHeld(VK_CONTROL))
		{
			if (MainSingleton::GetInputManager().IsKeyPressed('S'))
			{
				SaveActiveScene();
			}
		}

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Scene##SceneMenuBar"))
			{
				if (ImGui::MenuItem("Save", "Ctrl + S"))
				{
					SaveActiveScene();
				}

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

				if (ImGui::MenuItem("Reload##SceneMenuItem"))
				{
					ReloadScene();
				}

				static float resetCurrentSceneTooltipDelayTimer = 0.0f;
				ShowHoveredToolTips("Reset current scene", resetCurrentSceneTooltipDelayTimer);

				if (ImGui::MenuItem("Set As Start##SceneMenuItem"))
				{
					SetActiveSceneAsStart();
				}

				static float setSceneAsStartTooltipDelayTimer = 0.0f;
				ShowHoveredToolTips("Open this scene on startup", setSceneAsStartTooltipDelayTimer);

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void SceneMenuBar::Draw()
	{
	}

	void SceneMenuBar::SaveActiveScene()
	{
		ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
		ECS::EntityComponentSystem::SaveData(ecs, MainSingleton::GetSceneManager().GetCurrentSceneInfo()->relativePath);
	}

	void SceneMenuBar::ShowSceneList()
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

	void SceneMenuBar::CreateNewScene()
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
				static float createNewSceneAsCopyTooltipDelayTimer = 0.0f;
				ShowHoveredToolTips("Create new copy of this scene", createNewSceneAsCopyTooltipDelayTimer);
			}
		}
	}

	void SceneMenuBar::ReloadScene()
	{
		Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();
		sceneManager.ReloadSceneFromFile(sceneManager.GetCurrentSceneInfo()->relativePath);
	}

	void SceneMenuBar::SetActiveSceneAsStart()
	{
		nlohmann::json jsonData = SimpleUtilities::FileManager::GetDataAsJson(SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME));
		jsonData["Game_Settings"]["Start_Scene_RelativePath"] = MainSingleton::GetSceneManager().GetCurrentSceneInfo()->relativePath;

		std::ofstream writeFile(SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME));
		assert(writeFile.is_open() && "Failed to open the file");

		writeFile << jsonData;
		writeFile.close();
	}

	void SceneMenuBar::ShowHoveredToolTips(const std::string& aToolTipText, float& aTimer)
	{
		if (ImGui::IsItemHovered())
		{
			aTimer += Global::GetDeltaTime();

			if (aTimer > 0.33f)
			{
				if (ImGui::BeginTooltip())
				{
					ImGui::Text(aToolTipText.c_str());
					ImGui::EndTooltip();
				}
			}
		}
		else
		{
			aTimer = 0.0f;
		}
	}
}