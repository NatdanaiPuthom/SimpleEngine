#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Game/Managers/ImGuiManager/Tools/Camera.hpp"

namespace Tool
{
	Camera::Camera()
	{
	}

	void Camera::Draw()
	{
		if (ImGui::Begin("Camera Controls"))
		{
			Simple::GraphicsEngine* graphicsEngine = SimpleGlobal::GetGraphicsEngine();
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
			ImGui::Text("Click and Hold to select the object. Move object using mouse");

			ImGui::Text("FreeFly      - TAB");
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(107, 0));
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f);
			float rotationSpeed = graphicsEngine->GetCamera()->GetRotationSpeed();
			if (ImGui::SliderFloat("Camera Rotate Speed", &rotationSpeed, 0.01f, 300.0f, "%0.3f"))
			{
				graphicsEngine->GetCamera()->SetRotateSpeed(rotationSpeed);
			}

			ImGui::SameLine();
			ImGui::Dummy(ImVec2(30, 0));
			ImGui::SameLine();
			ImGui::Text("While object is selected, hold CTRL to move object up/down.");

			ImGui::Text("Speedx5      - Hold Shift");

			ImGui::SameLine();
			ImGui::Dummy(ImVec2(106, 0));
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f);
			float fov = graphicsEngine->GetCamera()->GetFoV() * globalRadToDeg;
			if (ImGui::SliderFloat("FoV", &fov, 0.0f, 180.00f, "%0.2f"))
			{
				fov *= globalDegToRad;
				graphicsEngine->GetCamera()->SetFoV(fov);
			}

			ImGui::Text("Fly Up       - SPACE");
			ImGui::Text("Fly Down     - Hold Ctrl + SPACE");
			ImGui::Text("Rotate Left  - E");
			ImGui::Text("Rotate Right - Q");
			ImGui::Text("Rotate Up    - C");
			ImGui::Text("Rotate Down  - Z");
		}
		ImGui::End();
	}
}