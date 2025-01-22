#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/DeferredPopUp.hpp"
#include "Engine/Global.hpp"

namespace Editor
{
	DeferredPopUp::DeferredPopUp(const std::string& aName) : PopUp(aName)
	{
	}

	void DeferredPopUp::Render()
	{
		static constexpr float aspectRatio = 16.0f / 9.0f; //TO-DO(v10.0.4): should read from some json file

		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		Graphics::RenderTargetManager* const renderTargetManager = graphicsEngine->GetRenderTargetManager();

		const ImVec2 screenSize = { static_cast<float>(Global::GetResolution().x), static_cast<float>(Global::GetResolution().y) };
		const float quadWidth = screenSize.x / 3.0f;
		const float quadHeight = screenSize.y / 2.0f;

		ImVec2 nextWindowPosition = ImGui::GetMainViewport()->WorkPos;
		const ImGuiWindowFlags imguiFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

		for (size_t i = 0; i < Graphics::Global_GBuffer_Count; ++i)
		{
			std::string name;

			switch (i)
			{
			case 0:
				name = "Albedo";
				break;
			case 1:
				name = "Normal";
				break;
			case 2:
				name = "Material";
				break;
			case 3:
				name = "Position";
				break;
			case 4:
				name = "AmbientOcclusion";
				break;
			default:
				name = "Unknown" + std::to_string(i);
				break;
			}

			ImGui::SetNextWindowSize(ImVec2(quadWidth + 2, quadHeight), ImGuiCond_Always);
			ImGui::SetNextWindowPos(nextWindowPosition);

			if ((i + 1) % 3 == 2)
			{
				nextWindowPosition.x += quadWidth - 1;
			}
			else if ((i + 1) % 3 != 0)
			{
				nextWindowPosition.x += quadWidth;
			}
			else
			{
				nextWindowPosition.x = ImGui::GetMainViewport()->WorkPos.x;
				nextWindowPosition.y += quadHeight - 19;
			}

			if (i < 3)
			{
				ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.18f, 0.18f, 0.18f, 0.8f));
			}

			if (ImGui::Begin(name.c_str(), nullptr, imguiFlags))
			{
				float newWidth = quadWidth;
				float newHeight = newWidth / aspectRatio;

				if (newHeight > quadHeight)
				{
					newHeight = quadHeight;
					newWidth = newHeight * aspectRatio;
				}

				ImTextureID texture = renderTargetManager->GetShaderResourceView(Graphics::eRenderTargetType::GBuffer, i).Get();
				ImGui::Image(texture, ImVec2(newWidth, newHeight));
			}
			ImGui::PopStyleColor();

			ImGui::End();
		}

		ImGui::SetNextWindowSize(ImVec2(quadWidth + 2, quadHeight), ImGuiCond_Always);
		ImGui::SetNextWindowPos(nextWindowPosition);

		if (ImGui::Begin("Deferred", nullptr, imguiFlags))
		{
			float newWidth = quadWidth;
			float newHeight = newWidth / aspectRatio;

			if (newHeight > quadHeight)
			{
				newHeight = quadHeight;
				newWidth = newHeight * aspectRatio;
			}

			ImTextureID texture = renderTargetManager->GetShaderResourceView(Graphics::eRenderTargetType::Deferred, 0).Get();
			ImGui::Image(texture, ImVec2(newWidth, newHeight));
		}

		ImGui::End();
	}
}
