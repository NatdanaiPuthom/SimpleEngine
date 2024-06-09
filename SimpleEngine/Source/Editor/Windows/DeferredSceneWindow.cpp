#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Windows/DeferredSceneWindow.hpp"

namespace Editor
{
	Editor::DeferredSceneWindow::DeferredSceneWindow()
	{
	}

	void Editor::DeferredSceneWindow::Init()
	{
	}

	void Editor::DeferredSceneWindow::Update()
	{
	}

	void Editor::DeferredSceneWindow::Draw()
	{
		static constexpr float aspectRatio = 16.0f / 9.0f; //TO-DO(v10.0.4): should read from some json file

		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		const ImVec2 screenSize = {static_cast<float>(Global::GetResolution().x), static_cast<float>(Global::GetResolution().y)};
		const float quadWidth = screenSize.x / 3.0f;
		const float quadHeight = screenSize.y / 2.0f;

		for (size_t i = 0; i < Graphics::Global_GBuffer_Count; ++i)
		{
			std::string name;

			switch (i)
			{
			case 0:
				name = "Albedo";
				break;
			case 2:
				name = "Material";
				break;
			case 1:
				name = "Normal";
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

			ImGui::SetNextWindowSize(ImVec2(quadWidth, quadHeight));

			if (ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
			{
				float newWidth = quadWidth;
				float newHeight = newWidth / aspectRatio;

				if (newHeight > quadHeight)
				{
					newHeight = quadHeight;
					newWidth = newHeight * aspectRatio;
				}

				ImTextureID texture = graphicsEngine->GetShaderResourceView(Graphics::eRenderTargetType::GBuffer, i).Get();
				ImGui::Image(texture, ImVec2(newWidth, newHeight));
			}

			ImGui::End();
		}

		ImGui::SetNextWindowSize(ImVec2(quadWidth, quadHeight), ImGuiCond_Always);

		if (ImGui::Begin("Deferred", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			float newWidth = quadWidth;
			float newHeight = newWidth / aspectRatio;

			if (newHeight > quadHeight)
			{
				newHeight = quadHeight;
				newWidth = newHeight * aspectRatio;
			}

			ImTextureID texture = graphicsEngine->GetShaderResourceView(Graphics::eRenderTargetType::Deferred, 0).Get();
			ImGui::Image(texture, ImVec2(newWidth, newHeight));
		}

		ImGui::End();
	}
}