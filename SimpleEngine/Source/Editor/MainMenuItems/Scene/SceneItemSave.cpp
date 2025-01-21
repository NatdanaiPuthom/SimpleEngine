#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MainMenuItems/Scene/SceneItemSave.hpp"
#include "Engine/Debugger/Console/Console.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneItemSave::SceneItemSave(const std::string& aName) : MainMenuItem(aName)
	{
	}

	void SceneItemSave::Update()
	{
		if (MainSingleton::GetInputManager().IsKeyHeld(VK_CONTROL))
		{
			if (MainSingleton::GetInputManager().IsKeyPressed('S'))
			{
				SaveActiveScene();
			}
		}
	}

	void SceneItemSave::OnClick()
	{
		SaveActiveScene();
	}

	void SceneItemSave::SaveActiveScene()
	{
		const Simpleton::SceneInfo* sceneInfo = MainSingleton::GetSceneManager().GetCurrentSceneInfo();
		ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
		ECS::EntityComponentSystem::SaveData(ecs, sceneInfo->relativePath);

		Simple::Console::Print("Scene: ", Simple::ConsoleTextColor::White, false);
		Simple::Console::Print(sceneInfo->name.c_str(), Simple::ConsoleTextColor::Green, false);
		Simple::Console::Print(" has been saved!", Simple::ConsoleTextColor::White, true);
	}
}