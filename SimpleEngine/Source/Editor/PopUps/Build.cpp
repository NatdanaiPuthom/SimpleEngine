#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Build.hpp"

namespace Editor
{
	Build::Build(const std::string& aName)
		: PopUp(aName)
	{
	}

	void Build::Render()
	{
		if (ImGui::Begin(myImGuiName.c_str()))
		{

		}

		ImGui::End();
	}
}
