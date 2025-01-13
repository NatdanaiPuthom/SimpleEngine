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
		Graphics::GraphicsEngine* const graphicsEngine = Global::GetGraphicsEngine();
		Graphics::LightManager* const graphicsDataContainer = graphicsEngine->GetLightManager();
		const Graphics::PostProcessData* const postProcessData = graphicsDataContainer->GetPostProcessData();

		if (ImGui::Begin("PostProcess##Windows", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			Math::Vector3f tintValue = postProcessData->tint;
			if (ImGui::ColorPicker3("Tint##PostProcessValue", &tintValue.x))
			{
				graphicsDataContainer->SetTint(tintValue);
			}

			float saturation = postProcessData->saturation;
			if (ImGui::DragFloat("Saturation##PostProcessValue", &saturation, 0.01f))
			{
				graphicsDataContainer->SetSaturation(saturation);
			}

			float exposure = postProcessData->exposure;
			if (ImGui::DragFloat("Exposure##PostProcessValue", &exposure, 0.01f))
			{
				graphicsDataContainer->SetExposure(exposure);
			}

			float contrast = postProcessData->contrast;
			if (ImGui::DragFloat("Contrast##PostProcessValue", &contrast, 0.01f))
			{
				graphicsDataContainer->SetContrast(contrast);
			}

			float blackpoint = postProcessData->blackpoint;
			if (ImGui::DragFloat("Blackpoint##PostProcessValue", &blackpoint, 0.01f))
			{
				graphicsDataContainer->SetBlackPoint(blackpoint);
			}

			ImGui::Separator();

			float bloom = postProcessData->bloom;
			if (ImGui::DragFloat("Bloom##PostProcessBloomValue", &bloom, 0.01f))
			{
				graphicsDataContainer->SetBloom(bloom);
			}

			float bloomPixelFilterThreshold = postProcessData->bloomPixelFilterThreshold;
			if (ImGui::DragFloat("Threshold##PostProcessBloomThreshold", &bloomPixelFilterThreshold, 0.001f))
			{
				graphicsDataContainer->SetBloomPixelThreshold(bloomPixelFilterThreshold);
			}

			ImGui::Separator();

			if (ImGui::Button("Reset##PostProcessReset"))
			{
				graphicsDataContainer->SetTint({1.0f, 1.0f, 1.0f});
				graphicsDataContainer->SetSaturation(1.0f);
				graphicsDataContainer->SetExposure(0.0f);
				graphicsDataContainer->SetContrast(1.0f);
				graphicsDataContainer->SetBlackPoint(0.0f);
				graphicsDataContainer->SetBloom(1.0f);
				graphicsDataContainer->SetBloomPixelThreshold(0.50f);
			}

			ImGui::SameLine();
			
			if (ImGui::Button("Default##PostProcessRestoreDefault"))
			{
				Graphics::PostProcessData tempPostProcessData;

				graphicsDataContainer->SetTint(tempPostProcessData.tint);
				graphicsDataContainer->SetSaturation(tempPostProcessData.saturation);
				graphicsDataContainer->SetExposure(tempPostProcessData.exposure);
				graphicsDataContainer->SetContrast(tempPostProcessData.contrast);
				graphicsDataContainer->SetBlackPoint(tempPostProcessData.blackpoint);
				graphicsDataContainer->SetBloom(tempPostProcessData.bloom);
				graphicsDataContainer->SetBloomPixelThreshold(tempPostProcessData.bloomPixelFilterThreshold);
			}

			ImGui::Separator();

			bool useToneMapping = static_cast<bool>(postProcessData->useToneMapping);
			if (ImGui::Checkbox("Use ACES Film ToneMapping##PostProcessUseToneMapping", &useToneMapping))
			{
				graphicsEngine->GetLightManager()->SetUseToneMapping(useToneMapping);
			}

			bool useBloom = static_cast<bool>(postProcessData->useBloom);
			if (ImGui::Checkbox("Use Bloom (does not work properly)##PostProcessUseBloom", &useBloom))
			{
				graphicsDataContainer->SetUseBloom(useBloom);
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