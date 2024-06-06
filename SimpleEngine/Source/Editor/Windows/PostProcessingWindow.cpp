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

		if (ImGui::Begin("PostProcess"))
		{
			ImGui::ColorPicker3("Tint", &graphicsEngine->myPostProcessData.tint.x);
			ImGui::DragFloat("Saturation", &graphicsEngine->myPostProcessData.saturation, 0.01f);
			ImGui::DragFloat("Exposure", &graphicsEngine->myPostProcessData.exposure, 0.01f);
			ImGui::DragFloat("Constract", &graphicsEngine->myPostProcessData.contrast, 0.01f);
			ImGui::DragFloat("Blackpoint", &graphicsEngine->myPostProcessData.blackpoint, 0.01f);
		}

		ImGui::End();
	}
}