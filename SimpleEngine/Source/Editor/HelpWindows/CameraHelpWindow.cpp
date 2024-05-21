#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/HelpWindows/CameraHelpWindow.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Engine/Global.hpp"

namespace Editor
{
	CameraHelpWindow::CameraHelpWindow()
	{
	}

	void CameraHelpWindow::Draw()
	{
		if (ImGui::Begin("Freefly Camera"))
		{
			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
			ImGui::Text("Movements    - WASD");
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(100, 0));
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f);

			float speed = graphicsEngine->GetCurrentCamera()->GetMoveSpeed();

			if (ImGui::DragFloat("MoveSpeed", &speed, 0.1f, 0.0f, 100.0f))
			{
				graphicsEngine->GetCurrentCamera()->SetMoveSpeed(speed);
			}

			ImGui::SameLine();
			ImGui::Dummy(ImVec2(100, 0));
			ImGui::SameLine();
			ImGui::Text("Click and Hold to select the scene object. Move object using mouse");

			ImGui::Text("FreeFly      - TAB");
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(107, 0));
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f);

			float rotationSpeed = graphicsEngine->GetCurrentCamera()->GetRotationSpeed();
			if (ImGui::DragFloat("Camera Rotate Speed", &rotationSpeed, 0.01f, 0.0f, 180.0f))
			{
				graphicsEngine->GetCurrentCamera()->SetRotateSpeed(rotationSpeed);
			}

			ImGui::SameLine();
			ImGui::Dummy(ImVec2(30, 0));
			ImGui::SameLine();
			ImGui::Text("While object is selected, hold CTRL to move object up/down.");

			ImGui::Text("Speedx5      - Hold Shift");

			ImGui::SameLine();
			ImGui::Dummy(ImVec2(58, 0));
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f);
			float fov = graphicsEngine->GetCurrentCamera()->GetHorizontalFoV() * Math::GLOBAL_RADIAN_TO_DEGREE;
			if (ImGui::SliderFloat("FoV", &fov, 0.0f, 180.00f, "%0.2f"))
			{
				fov *= Math::GLOBAL_DEGREE_TO_RADIAN;
				graphicsEngine->GetCurrentCamera()->SetHorizontalFoV(fov, Global::GetResolution());
			}

			ImGui::Text("Fly Up       - SPACE");
			ImGui::Text("Fly Down     - Hold Ctrl + SPACE");
			ImGui::Text("Rotate Left  - E");
			ImGui::Text("Rotate Right - Q");
			ImGui::Text("Rotate Up    - C");
			ImGui::Text("Rotate Down  - Z");
			ImGui::Text("Movespeed    - Scroll");
		}
		ImGui::End();
	}
}