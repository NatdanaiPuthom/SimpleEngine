#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/BuildMenuBar.hpp"

namespace Editor
{
	BuildMenuBar::BuildMenuBar()
	{
	}

	BuildMenuBar::~BuildMenuBar()
	{
	}

	void BuildMenuBar::Init()
	{
	}

	void BuildMenuBar::Update()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Build##BuildMenuBar"))
			{
				ImGui::Text("v11.4.1: Does not work yet");
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void BuildMenuBar::Draw()
	{
		
	}
}