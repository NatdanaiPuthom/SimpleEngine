#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MenuItems/Scene/SceneItemSaveButton.hpp"
#include "Engine/Debugger/Console/Console.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneItemSaveButton::SceneItemSaveButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName)
	{
	}

	void SceneItemSaveButton::Invoke()
	{
		const Simpleton::SceneInfo* sceneInfo = MainSingleton::GetSceneManager().GetCurrentSceneInfo();
		ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
		ECS::EntityComponentSystem::SaveData(ecs, sceneInfo->relativePath);

		Simple::Console::Print("Scene ", Simple::ConsoleTextColor::White, false);
		Simple::Console::Print(sceneInfo->name.c_str(), Simple::ConsoleTextColor::Green, false);
		Simple::Console::Print(" has been saved!", Simple::ConsoleTextColor::White, true);
	}
}
