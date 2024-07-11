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

		if (ImGui::Begin("PostProcess##Windows", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			Math::Vector3f tintValue = postProcessData.tint;
			if (ImGui::ColorPicker3("Tint##PostProcessValue", &tintValue.x))
			{
				graphicsEngine->SetTint(tintValue);
			}

			float saturation = postProcessData.saturation;
			if (ImGui::DragFloat("Saturation##PostProcessValue", &saturation, 0.01f))
			{
				graphicsEngine->SetSaturation(saturation);
			}

			float exposure = postProcessData.exposure;
			if (ImGui::DragFloat("Exposure##PostProcessValue", &exposure, 0.01f))
			{
				graphicsEngine->SetExposure(exposure);
			}

			float contrast = postProcessData.contrast;
			if (ImGui::DragFloat("Contrast##PostProcessValue", &contrast, 0.01f))
			{
				graphicsEngine->SetContrast(contrast);
			}

			float blackpoint = postProcessData.blackpoint;
			if (ImGui::DragFloat("Blackpoint##PostProcessValue", &blackpoint, 0.01f))
			{
				graphicsEngine->SetBlackPoint(blackpoint);
			}

			ImGui::Separator();

			float bloom = postProcessData.bloom;
			if (ImGui::DragFloat("Bloom##PostProcessBloomValue", &bloom, 0.01f))
			{
				graphicsEngine->SetBloom(bloom);
			}

			float bloomPixelFilterThreshold = postProcessData.bloomPixelFilterThreshold;
			if (ImGui::DragFloat("Threshold##PostProcessBloomThreshold", &bloomPixelFilterThreshold, 0.001f))
			{
				graphicsEngine->SetBloomPixelThreshold(bloomPixelFilterThreshold);
			}

			ImGui::Separator();

			if (ImGui::Button("Reset##PostProcessReset"))
			{
				graphicsEngine->SetTint({1.0f, 1.0f, 1.0f});
				graphicsEngine->SetSaturation(1.0f);
				graphicsEngine->SetExposure(0.0f);
				graphicsEngine->SetContrast(1.0f);
				graphicsEngine->SetBlackPoint(0.0f);
				graphicsEngine->SetBloom(1.0f);
				graphicsEngine->SetBloomPixelThreshold(0.50f);
			}

			ImGui::SameLine();
			
			if (ImGui::Button("Default##PostProcessRestoreDefault"))
			{
				PostProcessData tempPostProcessData;

				graphicsEngine->SetTint(tempPostProcessData.tint);
				graphicsEngine->SetSaturation(tempPostProcessData.saturation);
				graphicsEngine->SetExposure(tempPostProcessData.exposure);
				graphicsEngine->SetContrast(tempPostProcessData.contrast);
				graphicsEngine->SetBlackPoint(tempPostProcessData.blackpoint);
				graphicsEngine->SetBloom(tempPostProcessData.bloom);
				graphicsEngine->SetBloomPixelThreshold(tempPostProcessData.bloomPixelFilterThreshold);
			}

			ImGui::Separator();

			bool useToneMapping = static_cast<bool>(postProcessData.useToneMapping);
			if (ImGui::Checkbox("Use ACES Film ToneMapping##PostProcessUseToneMapping", &useToneMapping))
			{
				graphicsEngine->SetUseToneMapping(useToneMapping);
			}

			bool useBloom = static_cast<bool>(postProcessData.useBloom);
			if (ImGui::Checkbox("Use Bloom (does not work properly)##PostProcessUseBloom", &useBloom))
			{
				graphicsEngine->SetUseBloom(useBloom);
			}
		}

		ImGui::End();

		if (ImGui::Begin("PostProcessImage##PostProcessImage", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
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

		if (ImGui::Begin("BloomUpDownScale##PostProcessImage"))
		{
			for (size_t i = 0; i < 5; ++i)
			{
				ImTextureID texture = graphicsEngine->GetShaderResourceView(Graphics::eRenderTargetType::BloomDownAndUpScale, 0).Get();
				ImGui::Image(texture, ImVec2(325, 325));

				if ((i + 1 ) % 3 != 0)
				{
					ImGui::SameLine();
				}
			}
		
			ImGui::SameLine();

			ImTextureID bloomTexture = graphicsEngine->GetShaderResourceView(Graphics::eRenderTargetType::Bloom, 0).Get();
			ImGui::Image(bloomTexture, ImVec2(325, 325));
		}
		ImGui::End();
	}
}