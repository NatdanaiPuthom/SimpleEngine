#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MainMenuItems/Scene/SceneItemReload.hpp"
#include "Engine/Debugger/Console/Console.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneItemReload::SceneItemReload(const std::string& aName) : MainMenuItem(aName)
	{
	}

	void SceneItemReload::OnClick()
	{
		const Simpleton::SceneInfo* sceneInfo = MainSingleton::GetSceneManager().GetCurrentSceneInfo();
		Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();
		sceneManager.ReloadSceneFromFile(sceneInfo->relativePath);

		Simple::Console::Print("Scene: ", Simple::ConsoleTextColor::White, false);
		Simple::Console::Print(sceneInfo->name.c_str(), Simple::ConsoleTextColor::Green, false);
		Simple::Console::Print(" has been reloaded!", Simple::ConsoleTextColor::White, true);
	}

	void SceneItemReload::ToolTips()
	{
		ImGui::Text("Reset current scene");
	}
}
