#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MenuItems/Scene/SceneItemSetAsStartButton.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "Engine/Debugger/Console/Console.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneItemSetAsStartButton::SceneItemSetAsStartButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName)
	{
	}

	void SceneItemSetAsStartButton::Invoke()
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
	}
}
