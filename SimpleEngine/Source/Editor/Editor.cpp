#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Editor.hpp"

#ifndef _SIMPLE
#include "Graphics/Renderer/Renderer.hpp"
#include "Editor/Menu/MainMenuBar.hpp"
#include "Editor/Menu/HelpMenuBar.hpp"
#endif

namespace Editor
{
#ifndef _SIMPLE
	EditorEngine::EditorEngine()
	{
		AddTool(std::move(std::make_unique<MainMenuBar>()));
		AddTool(std::move(std::make_unique<HelpMenuBar>()));
	}

	EditorEngine::~EditorEngine()
	{
	}

	void EditorEngine::Init()
	{
		for (const auto& tool : myTools)
		{
			tool->Init();
		}
	}

	void EditorEngine::Update()
	{
		Global::GetGraphicsEngine()->GetEditorCamera()->Update(Global::GetDeltaTime(), Global::GetEngineHWND());

		if (MainSingleton::GetInputManager().IsKeyPressed(VK_F5))
		{
			if (Global::IsFullScreen())
			{
				Global::SetWindowSizeNextFrame({ 1280,720 }, false);
			}
			else
			{
				Global::SetWindowSizeNextFrame({ 0,0 }, true);
			}
		}

		for (const std::unique_ptr<ToolInterface>& tool : myTools)
		{
			if (tool->myIsActive == true)
			{
				tool->Update();
			}
		}

		for (const std::unique_ptr<ToolInterface>& tool : myTools)
		{
			if (tool->myIsActive == true)
			{
				tool->Draw();
			}
		}

		/*Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		static Math::Vector4f ambientLight = graphicsEngine->GetAmbientLightColorAndIntensity();
		static Math::Vector4f pointLightColorAndIntensity1 = { 1.0f, 0.0f, 0.0f, 100.0f };
		static Math::Vector3f pointLightPosition = { -5.0f, 5.5f, 4.2f};
		static float range = 1.0f;

		if (ImGui::Begin("Light"))
		{
			if (ImGui::DragFloat4("Ambient light", &ambientLight.x, 0.1f))
			{
				graphicsEngine->SetAmbientLightColorAndIntensity(ambientLight);
			}

			ImGui::Separator();

			if (ImGui::DragFloat4("PointLight1 Color", &pointLightColorAndIntensity1.x, 0.1f))
			{
			}

			if (ImGui::DragFloat4("PointLight1 Position", &pointLightPosition.x, 0.1f))
			{
			}

			if (ImGui::DragFloat("PointLight1 Range", &range, 0.1f))
			{
			}
		}
		ImGui::End();*/

		/*PointLightData pointlight;
		pointlight.color = pointLightColorAndIntensity1;
		pointlight.position = pointLightPosition;
		pointlight.range = range;

		graphicsEngine->AddPointLight(pointlight);*/
	}

	void EditorEngine::Render()
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
			case 1:
				name = "Material";
				break;
			case 2:
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

	void EditorEngine::AddTool(std::unique_ptr<ToolInterface> aTool)
	{
		myTools.push_back(std::move(aTool));
	}

#else
	EditorEngine::EditorEngine() {}
	EditorEngine::~EditorEngine() {}

	void EditorEngine::Init() {}
	void EditorEngine::Update() {}
	void EditorEngine::Render() {}

	void EditorEngine::AddTool(std::unique_ptr<ToolInterface> /*aTool*/) {}
#endif
}