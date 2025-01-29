#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Editor/SceneInspectorPopUp.hpp"

namespace Editor
{
	SceneInspectorPopUp::SceneInspectorPopUp(const std::string& aName)
		: PopUp(aName)
	{
	}

	void SceneInspectorPopUp::Render()
	{
		if (ImGui::Begin(myImGuiName.c_str(), &myIsActive))
		{
			
		}

		ImGui::End();
	}
}
