#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/PostProcessPopUp.hpp"

namespace Editor
{
	PostProcessPopUp::PostProcessPopUp(const std::string& aName) : PopUp(aName)
	{
	}

	void PostProcessPopUp::Render()
	{
		Graphics::GraphicsEngine* const graphicsEngine = Global::GetGraphicsEngine();
		Graphics::LightManager* const graphicsLightManager = graphicsEngine->GetLightManager();
		Graphics::RenderTargetManager* const graphicsRenderTargetManager = graphicsEngine->GetRenderTargetManager();
		const Graphics::PostProcessData* const postProcessData = graphicsLightManager->GetPostProcessData();

		if (ImGui::Begin(myImGuiName.c_str()))
		{
			Math::Vector3f tintValue = postProcessData->tint;
			if (ImGui::ColorPicker3("Tint##PostProcessValue", &tintValue.x))
			{
				graphicsLightManager->SetTint(tintValue);
			}

			float saturation = postProcessData->saturation;
			if (ImGui::DragFloat("Saturation##PostProcessValue", &saturation, 0.01f))
			{
				graphicsLightManager->SetSaturation(saturation);
			}

			float exposure = postProcessData->exposure;
			if (ImGui::DragFloat("Exposure##PostProcessValue", &exposure, 0.01f))
			{
				graphicsLightManager->SetExposure(exposure);
			}

			float contrast = postProcessData->contrast;
			if (ImGui::DragFloat("Contrast##PostProcessValue", &contrast, 0.01f))
			{
				graphicsLightManager->SetContrast(contrast);
			}

			float blackpoint = postProcessData->blackpoint;
			if (ImGui::DragFloat("Blackpoint##PostProcessValue", &blackpoint, 0.01f))
			{
				graphicsLightManager->SetBlackPoint(blackpoint);
			}

			ImGui::Separator();

			float bloom = postProcessData->bloom;
			if (ImGui::DragFloat("Bloom##PostProcessBloomValue", &bloom, 0.01f))
			{
				graphicsLightManager->SetBloom(bloom);
			}

			float bloomPixelFilterThreshold = postProcessData->bloomPixelFilterThreshold;
			if (ImGui::DragFloat("Threshold##PostProcessBloomThreshold", &bloomPixelFilterThreshold, 0.001f))
			{
				graphicsLightManager->SetBloomPixelThreshold(bloomPixelFilterThreshold);
			}

			ImGui::Separator();

			if (ImGui::Button("Reset##PostProcessReset"))
			{
				graphicsLightManager->SetTint({1.0f, 1.0f, 1.0f});
				graphicsLightManager->SetSaturation(1.0f);
				graphicsLightManager->SetExposure(0.0f);
				graphicsLightManager->SetContrast(1.0f);
				graphicsLightManager->SetBlackPoint(0.0f);
				graphicsLightManager->SetBloom(1.0f);
				graphicsLightManager->SetBloomPixelThreshold(0.50f);
			}

			ImGui::SameLine();
			
			if (ImGui::Button("Default##PostProcessRestoreDefault"))
			{
				Graphics::PostProcessData tempPostProcessData;

				graphicsLightManager->SetTint(tempPostProcessData.tint);
				graphicsLightManager->SetSaturation(tempPostProcessData.saturation);
				graphicsLightManager->SetExposure(tempPostProcessData.exposure);
				graphicsLightManager->SetContrast(tempPostProcessData.contrast);
				graphicsLightManager->SetBlackPoint(tempPostProcessData.blackpoint);
				graphicsLightManager->SetBloom(tempPostProcessData.bloom);
				graphicsLightManager->SetBloomPixelThreshold(tempPostProcessData.bloomPixelFilterThreshold);
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
				graphicsLightManager->SetUseBloom(useBloom);
			}
		}

		ImGui::End();

		//if (ImGui::Begin("PostProcessImage##PostProcessImage", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		if (ImGui::Begin("PostProcessImage##PostProcessImage"))
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

			ImTextureID texture = graphicsRenderTargetManager->GetShaderResourceView(Graphics::eRenderTargetType::PostProcessing, 0).Get();
			ImGui::Image(texture, ImVec2(newWidth, newHeight));
		}

		ImGui::End();

		if (ImGui::Begin("BloomUpDownScale##PostProcessImage"))
		{
			for (size_t i = 0; i < 5; ++i)
			{
				ImTextureID texture = graphicsRenderTargetManager->GetShaderResourceView(Graphics::eRenderTargetType::BloomDownAndUpScale, 0).Get();
				ImGui::Image(texture, ImVec2(325, 325));

				if ((i + 1 ) % 3 != 0)
				{
					ImGui::SameLine();
				}
			}
		
			ImGui::SameLine();

			ImTextureID bloomTexture = graphicsRenderTargetManager->GetShaderResourceView(Graphics::eRenderTargetType::Bloom, 0).Get();
			ImGui::Image(bloomTexture, ImVec2(325, 325));
		}
		ImGui::End();
	}
}
