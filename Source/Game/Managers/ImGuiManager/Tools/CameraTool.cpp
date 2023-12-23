#include "Game/stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Game/Managers/ImGuiManager/Tools/CameraTool.hpp"

CameraTool::CameraTool()
{
}

void CameraTool::Draw()
{
	GraphicsEngine* graphicsEngine = SimpleGlobal::GetGraphicsEngine();

	if (ImGui::Begin("Camera Controls")) //TO-DO: Move All ImGui stuff to it's own place
	{
		ImGui::Text("Movements    - WASD");
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(100, 0));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);

		float speed = graphicsEngine->GetCamera()->GetMoveSpeed();
		if (ImGui::SliderFloat("MoveSpeed", &speed, 0, 100, "%0.2f"))
		{
			graphicsEngine->GetCamera()->SetMoveSpeed(speed);
		}

		ImGui::Text("FreeFly      - TAB");

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(106, 0));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);
		float fov = graphicsEngine->GetCamera()->GetFoV() * SimpleUtilities::GetRadToDeg();
		if (ImGui::SliderFloat("FoV", &fov, 0.0f, 180.00f, "%0.2f"))
		{
			fov *= SimpleUtilities::GetDegToRad();
			graphicsEngine->GetCamera()->SetFoV(fov);
		}

		ImGui::Text("Speedx3      - Hold Shift");
		ImGui::Text("Fly Up       - SPACE");
		ImGui::Text("Fly Down     - Hold Ctrl + SPACE");
		ImGui::Text("Rotate Left  - E");
		ImGui::Text("Rotate Right - Q");
		ImGui::Text("Rotate Up    - C");
		ImGui::Text("Rotate Down  - Z");
	}
	ImGui::End();

}
