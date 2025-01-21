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
		ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
		ECS::EntityComponentSystem::SaveData(ecs, MainSingleton::GetSceneManager().GetCurrentSceneInfo()->relativePath);
		Simple::Console::Print("Scene has been saved!", Simple::ConsoleTextColor::Green, true);
	}
}