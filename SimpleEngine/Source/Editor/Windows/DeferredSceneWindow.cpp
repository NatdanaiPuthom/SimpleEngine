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
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

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

			if (ImGui::Begin(name.c_str()))
			{
				const ImVec2 windowSize = ImGui::GetWindowSize();
				ImTextureID texture = graphicsEngine->GetShaderResourceView(Graphics::eRenderTargetType::GBuffer, i).Get();
				ImGui::Image(texture, windowSize);
			}

			ImGui::End();
		}

		if (ImGui::Begin("Deferred"))
		{
			const ImVec2 windowSize = ImGui::GetWindowSize();
			ImTextureID texture = graphicsEngine->GetShaderResourceView(Graphics::eRenderTargetType::Deferred, 0).Get();
			ImGui::Image(texture, windowSize);
		}

		ImGui::End();
	}
}