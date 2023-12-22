#include "Game/stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Game/Managers/ImGuiManager/Tools/CameraTool.hpp"

CameraTool::CameraTool()
	: mySelectedResolution(1)
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

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(100, 0));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);
		float fov = graphicsEngine->GetCamera()->GetFoV() * SimpleUtilities::GetRadToDeg();
		if (ImGui::SliderFloat("FoV", &fov, 0.0f, 180.00f, "%0.2f"))
		{
			fov *= SimpleUtilities::GetDegToRad();
			graphicsEngine->GetCamera()->SetFoV(fov);
		}

		ImGui::Text("FreeFly      - TAB");

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(107, 0));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(200.0f);

		std::vector<SimpleUtilities::Vector2i> resolutions =
		{
			SimpleUtilities::Vector2i(800, 600),
			SimpleUtilities::Vector2i(1280, 720),
			SimpleUtilities::Vector2i(1920, 1080),
		};

		const char* resolutionText[] = { "800x600", "1280x720", "1920x1080" };
		if (ImGui::Combo("Resolution", &mySelectedResolution, resolutionText, 3))
		{
			SimpleUtilities::Vector2f resolution(static_cast<float>(resolutions[mySelectedResolution].x), static_cast<float>(resolutions[mySelectedResolution].y));
			graphicsEngine->GetCamera()->SetResolution(resolution);
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
