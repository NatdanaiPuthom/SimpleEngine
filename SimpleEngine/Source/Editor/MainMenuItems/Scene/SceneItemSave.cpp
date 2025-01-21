#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MainMenuItems/Scene/SceneItemSave.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneItemSave::SceneItemSave(const std::string& aName) : MainMenuItem(aName)
	{
	}

	void SceneItemSave::OnClick()
	{
		ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
		ECS::EntityComponentSystem::SaveData(ecs, MainSingleton::GetSceneManager().GetCurrentSceneInfo()->relativePath);
	}
}