#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/FileMenuBar.hpp"

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
				if (ImGui::MenuItem("Open", ""))
				{
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