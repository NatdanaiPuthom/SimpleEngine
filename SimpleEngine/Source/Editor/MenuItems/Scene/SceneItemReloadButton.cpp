#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MenuItems/Scene/SceneItemReloadButton.hpp"
#include "Engine/Debugger/Console/Console.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneItemReloadButton::SceneItemReloadButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName)
	{
	}

	void SceneItemReloadButton::Invoke()
	{
		Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();
		sceneManager.ReloadSceneFromFile(sceneManager.GetCurrentSceneInfo()->relativePath);

		Simple::Console::Print("Scene ", Simple::ConsoleTextColor::White, false);
		Simple::Console::Print(sceneManager.GetCurrentSceneInfo()->name.c_str(), Simple::ConsoleTextColor::Green, false);
		Simple::Console::Print(" has been reloaded!", Simple::ConsoleTextColor::White, true);
	}
}
