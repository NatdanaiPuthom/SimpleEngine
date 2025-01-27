#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Settings/CameraSettingsPopUp.hpp"
#include "Engine/Global.hpp"

namespace Editor
{
	CameraSettingsPopUp::CameraSettingsPopUp(const std::string& aName)
		: PopUp(aName)
	{
	}

	void CameraSettingsPopUp::Render()
	{
		if (ImGui::Begin(myImGuiName.c_str(), &myIsActive))
		{
			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

			float speed = graphicsEngine->GetCurrentCamera()->GetMoveSpeed();
			float rotationSpeed = graphicsEngine->GetCurrentCamera()->GetRotationSpeed();
			float fov = graphicsEngine->GetCurrentCamera()->GetHorizontalFoV() * Math::GLOBAL_RADIAN_TO_DEGREE;

			if (ImGui::DragFloat("Camera Move Speed", &speed, 0.1f, 0.0f, 100.0f))
			{
				graphicsEngine->GetCurrentCamera()->SetMoveSpeed(speed);
			}

			if (ImGui::DragFloat("Camera Rotate Speed", &rotationSpeed, 0.01f, 0.0f, 180.0f))
			{
				graphicsEngine->GetCurrentCamera()->SetRotateSpeed(rotationSpeed);
			}

			if (ImGui::SliderFloat("FoV", &fov, 0.01f, 179.00f, "%0.2f"))
			{
				fov *= Math::GLOBAL_DEGREE_TO_RADIAN;
				graphicsEngine->GetCurrentCamera()->SetHorizontalFoV(fov, Global::GetResolution());
			}
		}

		ImGui::End();
	}
}