#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Help/CameraControlsGuidePopUp.hpp"

namespace Editor
{
	CameraControlsGuidePopUp::CameraControlsGuidePopUp(const std::string& aName)
		: PopUp(aName)
	{
	}

	void CameraControlsGuidePopUp::Render()
	{
		if (ImGui::Begin(myImGuiName.c_str(), &myIsActive))
		{
			ImGui::Text("FreeFly               - TAB");
			ImGui::Text("Movements             - W/A/S/D");
			ImGui::Text("Rotate Left           - E");
			ImGui::Text("Rotate Right          - Q");
			ImGui::Text("Rotate Up             - C");
			ImGui::Text("Rotate Down           - Z");
			ImGui::Text("Fly Up                - SPACE");
			ImGui::Text("Fly Down              - Hold Ctrl + SPACE");
			ImGui::Text("Speed x5              - Hold Shift");
			ImGui::Text("LookAround            - Hold Right-Click");
			ImGui::Text("Increase Movespeed    - Scroll");
		}

		ImGui::End();
	}
}
