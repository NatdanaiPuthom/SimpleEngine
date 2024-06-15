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
				auto ecs = World::GetECS();
				ECS::EntityComponentSystem::SaveData(*ecs, "Assets/Scenes/Test_Scene.scene");
			}
		}

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File##FileMenuBar"))
			{
				if (ImGui::MenuItem("Save", "Ctrl + S"))
				{
					auto ecs = World::GetECS();
					ECS::EntityComponentSystem::SaveData(*ecs, "Assets/Scenes/Test_Scene.scene"); //TO-DO(v11.1.0): save current active scene
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void FileMenuBar::Draw()
	{
	}
}