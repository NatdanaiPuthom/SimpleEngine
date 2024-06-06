#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Windows/PostProcessingWindow.hpp"

namespace Editor
{
	Editor::PostProcessingWindow::PostProcessingWindow()
	{
	}

	void Editor::PostProcessingWindow::Init()
	{
	}

	void Editor::PostProcessingWindow::Update()
	{
	}

	void Editor::PostProcessingWindow::Draw()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		const PostProcessData& postProcessData = graphicsEngine->GetPostProcessData();

		if (ImGui::Begin("PostProcess", 0, ImGuiWindowFlags_NoMove))
		{
			Math::Vector3f tintValue = postProcessData.tint;
			if (ImGui::ColorPicker3("Tint", &tintValue.x))
			{
				graphicsEngine->SetTint(tintValue);
			}

			float saturation = postProcessData.saturation;
			if (ImGui::DragFloat("Saturation", &saturation, 0.01f))
			{
				graphicsEngine->SetSaturation(saturation);
			}

			float exposure = postProcessData.exposure;
			if (ImGui::DragFloat("Exposure", &exposure, 0.01f))
			{
				graphicsEngine->SetExposure(exposure);
			}

			float contrast = postProcessData.contrast;
			if (ImGui::DragFloat("Constract", &contrast, 0.01f))
			{
				graphicsEngine->SetContrast(contrast);
			}

			float blackpoint = postProcessData.blackpoint;
			if (ImGui::DragFloat("Blackpoint", &blackpoint, 0.01f))
			{
				graphicsEngine->SetBlackPoint(blackpoint);
			}
		}

		ImGui::End();

		if (ImGui::Begin("PostProcessImage", 0, ImGuiWindowFlags_NoMove))
		{
			const ImVec2 windowSize = ImGui::GetWindowSize();

			const float aspectRatio = 16.0f / 9.0f;

			float newWidth = windowSize.x;
			float newHeight = newWidth / aspectRatio;

			if (newHeight > windowSize.y)
			{
				newHeight = windowSize.y;
				newWidth = newHeight * aspectRatio;
			}

			ImTextureID texture = graphicsEngine->GetShaderResourceView(Graphics::eRenderTargetType::PostProcessing, 0).Get();
			ImGui::Image(texture, ImVec2(newWidth, newHeight));
		}

		ImGui::End();
	}
}