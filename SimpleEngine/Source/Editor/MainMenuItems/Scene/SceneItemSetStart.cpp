#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MainMenuItems/Scene/SceneItemSetStart.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "Engine/Debugger/Console/Console.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneItemSetAsStart::SceneItemSetAsStart(const std::string& aName) : MainMenuItem(aName)
	{
	}

	void SceneItemSetAsStart::OnClick()
	{
		const Simpleton::SceneInfo* sceneInfo = MainSingleton::GetSceneManager().GetCurrentSceneInfo();

		nlohmann::json jsonData = SimpleUtilities::FileManager::GetDataAsJson(SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME));
		jsonData["Game_Settings"]["Start_Scene_RelativePath"] = sceneInfo->relativePath;

		std::ofstream writeFile(SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME));
		assert(writeFile.is_open() && "Failed to open the file");

		writeFile << jsonData;
		writeFile.close();

		Simple::Console::Print("Scene: ", Simple::ConsoleTextColor::White, false);
		Simple::Console::Print(sceneInfo->name.c_str(), Simple::ConsoleTextColor::Green, false);
		Simple::Console::Print(" has been set as startup!", Simple::ConsoleTextColor::White, true);
	}

	void SceneItemSetAsStart::ToolTips()
	{
		ImGui::Text("Open current scene on startup");
	}
}
