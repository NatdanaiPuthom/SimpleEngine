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

				if (ImGui::MenuItem("Create##SceneMenuItem"))
				{
					CreateNewScene();
				}

				if (ImGui::MenuItem("Reload##SceneMenuItem"))
				{
					ReloadScene();
				}

				ShowReloadTooltips();

				if (ImGui::MenuItem("Set As Start##SceneMenuItem"))
				{
					SetActiveSceneAsStart();
				}

				ShowSetStartToolTips();

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
		Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();

		const std::string absolutePath = SimpleUtilities::GetAbsolutePath(SimpleUtilities::AppendCounterIfAlreadyExist(std::string(SIMPLE_DIR_SCENES) + "\\" + std::string(SIMPLE_FILENAME_NEWSCENE)));
		const std::string relativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(absolutePath);
		sceneManager.CreateNewScene(absolutePath);
		sceneManager.ChangeScene(relativePath);
	}

	void SceneMenuBar::ReloadScene()
	{
		Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();
		sceneManager.ReloadSceneFromFile(sceneManager.GetCurrentSceneInfo()->relativePath);
	}

	void SceneMenuBar::ShowReloadTooltips()
	{
		static float timer = 0.0f;

		if (ImGui::IsItemHovered())
		{
			timer += Global::GetDeltaTime();

			if (timer > 0.33f)
			{
				if (ImGui::BeginTooltip())
				{
					ImGui::Text("Reset current scene");
					ImGui::EndTooltip();
				}
			}
		}
		else
		{
			timer = 0.0f;
		}
	}

	void SceneMenuBar::ShowSetStartToolTips()
	{
		static float timer = 0.0f;

		if (ImGui::IsItemHovered())
		{
			timer += Global::GetDeltaTime();

			if (timer > 0.33f)
			{
				if (ImGui::BeginTooltip())
				{
					ImGui::Text("Open this scene on startup");
					ImGui::EndTooltip();
				}
			}
		}
		else
		{
			timer = 0.0f;
		}
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
}