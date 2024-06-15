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
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File##FileMenuBar"))
			{
				if (ImGui::MenuItem("Save", "Ctrl + S"))
				{
					auto ecs = World::GetECS();
					ECS::EntityComponentSystem::SaveData(*ecs, "Assets/Scenes/Test_Scene_Copy.scene");
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