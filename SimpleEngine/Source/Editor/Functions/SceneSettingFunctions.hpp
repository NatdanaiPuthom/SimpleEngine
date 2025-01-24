#pragma once
#include "Engine/Debugger/Console/Console.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include <functional>
#include <fstream>

namespace Editor
{
	class SceneSettingsFunction
	{
	public:
		static std::function<void()> Save()
		{
			return []() -> void
				{
					const Simpleton::SceneInfo* sceneInfo = MainSingleton::GetSceneManager().GetCurrentSceneInfo();
					ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
					ECS::EntityComponentSystem::SaveData(ecs, sceneInfo->relativePath);

					Simple::Console::Print("Scene ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(sceneInfo->name.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print(" has been saved!", Simple::ConsoleTextColor::White, true);
				};
		}

		static std::function<void(const std::string&)> Load()
		{
			return [](const std::string& aString) -> void
				{
					const std::string scenePath = std::string(SIMPLE_DIR_SCENES) + "\\" + aString;
					MainSingleton::GetSceneManager().ChangeScene(scenePath);
					Simple::Console::Print("Loaded scene ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(aString.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print("!", Simple::ConsoleTextColor::White, true);
				};
		}

		static std::function<void()> CreateNew()
		{
			return []() -> void
				{
					Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();

					const std::string absolutePath = SimpleUtilities::GetAbsolutePath(SimpleUtilities::AppendCounterIfAlreadyExist(std::string(SIMPLE_DIR_SCENES) + "\\" + std::string(SIMPLE_FILENAME_NEWSCENE)));
					const std::string relativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(absolutePath);

					sceneManager.CreateNewScene(absolutePath);
					sceneManager.ChangeScene(relativePath);

					Simple::Console::Print("New scene ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(sceneManager.GetCurrentSceneInfo()->name.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print(" has been created!", Simple::ConsoleTextColor::White, true);
				};
		}

		static std::function<void()> CreateCopy()
		{
			return []() -> void
				{
					Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();

					const std::string absolutePath = sceneManager.GetCurrentSceneInfo()->absolutePath;
					const std::string newCopyName = SimpleUtilities::AppendStringBeforeDot("_Copy", absolutePath);
					const std::string newFileName = SimpleUtilities::AppendCounterIfAlreadyExist(newCopyName);
					const std::string relativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(newFileName);

					std::filesystem::copy_file(absolutePath, newFileName, std::filesystem::copy_options::overwrite_existing);
					sceneManager.ChangeScene(relativePath);

					Simple::Console::Print("New scene ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(sceneManager.GetCurrentSceneInfo()->name.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print(" has been created!", Simple::ConsoleTextColor::White, true);
				};
		}

		static std::function<void()> Reload()
		{
			return []() -> void
				{
					Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();
					sceneManager.ReloadSceneFromFile(sceneManager.GetCurrentSceneInfo()->relativePath);

					Simple::Console::Print("Scene ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(sceneManager.GetCurrentSceneInfo()->name.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print(" has been reloaded!", Simple::ConsoleTextColor::White, true);
				};
		}

		static std::function<void()> SetAsActive()
		{
			return []() -> void
				{
					const Simpleton::SceneInfo* sceneInfo = MainSingleton::GetSceneManager().GetCurrentSceneInfo();
					nlohmann::json jsonData = SimpleUtilities::FileManager::GetDataAsJson(SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME));
					jsonData["Game_Settings"]["Start_Scene_RelativePath"] = sceneInfo->relativePath;

					std::ofstream writeFile(SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME));
					assert(writeFile.is_open() && "Failed to open the file");

					writeFile << jsonData;
					writeFile.close();

					Simple::Console::Print("Scene ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(sceneInfo->name.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print(" has been set as start!", Simple::ConsoleTextColor::White, true);
				};
		}
	};
}