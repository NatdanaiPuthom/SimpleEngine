#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/SceneMenuBar.hpp"
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

				ImGui::BeginDisabled();
				if (ImGui::MenuItem("Load"))
				{
					
				}
				ImGui::EndDisabled();

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
		ECS::EntityComponentSystem::SaveData(ecs, MainSingleton::GetSceneManager().GetCurrentScenePath());
	}
}