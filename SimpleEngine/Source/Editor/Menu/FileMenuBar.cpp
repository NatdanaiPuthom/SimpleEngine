#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/FileMenuBar.hpp"
#include "Game/World.hpp"

namespace Editor
{
	FileMenuBar::FileMenuBar()
	{
	}

	FileMenuBar::~FileMenuBar()
	{
	}

	void FileMenuBar::Init()
	{
	}

	void FileMenuBar::Update()
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

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void FileMenuBar::Draw()
	{
	}

	void FileMenuBar::SaveActiveScene()
	{
		ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
		ECS::EntityComponentSystem::SaveData(ecs, MainSingleton::GetSceneManager().GetCurrentScenePath());
	}
}