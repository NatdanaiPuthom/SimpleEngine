#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Editor/SceneWindowPopUp.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"
#include "Engine/Global.hpp"

namespace Editor
{
	SceneWindowPopUp::SceneWindowPopUp(const std::string& aName)
		: PopUp(aName)
		, myGridSnapTool(&myTransformEntityTool)
	{
	}

	void SceneWindowPopUp::Render()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 3));

		if (ImGui::Begin(myImGuiName.c_str(), &myIsActive, ImGuiWindowFlags_NoScrollbar))
		{
			RenderOrientationCube();

			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
			Graphics::RenderTargetManager* const renderTargetManager = graphicsEngine->GetRenderTargetManager();
			const Graphics::eRasterizerState currentRasterizerState = graphicsEngine->GetStateManager()->GetCurrentRasterizerState();

			ImTextureID textureID = renderTargetManager->GetShaderResourceView(Graphics::eRenderTargetType::PostProcessing).Get();

			if (currentRasterizerState != Graphics::eRasterizerState::BackfaceCulling
				&& currentRasterizerState != Graphics::eRasterizerState::NoFaceCulling)
			{
				textureID = renderTargetManager->GetShaderResourceView(Graphics::eRenderTargetType::Deferred).Get();
			}

			const ImVec2 size = ImGui::GetContentRegionAvail();
			ImGui::SetNextItemAllowOverlap();
			ImGui::Image(textureID, size);

			myTransformEntityTool.Render();

			// Popup currently in use for grid snapping. Might add more stuff later?
			{
				const ImVec2 previousCursorPos = ImGui::GetCursorPos();
				const ImVec2 sceneSettingsPopupPosition{ ImGui::GetWindowContentRegionMax().x - 70.f, ImGui::GetWindowContentRegionMin().y + 5.f };
				ImGui::SetCursorPos(sceneSettingsPopupPosition);

				if (ImGui::Button("Options", ImVec2{ 60.f, 25.f }))
				{
					ImGui::OpenPopup("Scene Settings##EditorSceneSettings");
				}

				ImGui::SetCursorPos(previousCursorPos);

				if (ImGui::BeginPopup("Scene Settings##EditorSceneSettings"))
				{
					myGridSnapTool.Render();
					ImGui::EndPopup();
				}
			}

		}

		ImGui::End();

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		if (ImGui::Begin(myImGuiName.c_str(), &myIsActive, ImGuiWindowFlags_NoScrollbar))
		{
			//TO-DO(v12.0.0): Make it render on Window's bar itself rather than MainMenuBar
			RenderPlayButton();
		}

		ImGui::End();
	}

	void SceneWindowPopUp::RenderOrientationCube()
	{
		//TO-DO(v12.0.0): The cube doesn't render when the window is pop out for unknown reason

		const Graphics::Camera* camera = Global::GetGraphicsEngine()->GetCurrentCamera();
		Math::Matrix4x4f view = camera->GetViewMatrix();

		constexpr float cubeSize = 64.0f;

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImVec2 cubePos = ImVec2(
			windowPos.x + windowSize.x - cubeSize - 10.0f,
			windowPos.y + windowSize.y - cubeSize - 10.0f
		);

		ImGuizmo::ViewManipulate(&view(1, 1), 16, cubePos, ImVec2(cubeSize, cubeSize), 0x00000000);
	}

	void SceneWindowPopUp::RenderPlayButton()
	{
		if (ImGui::BeginMainMenuBar())
		{
			const float distanceFromStart = ImGui::GetWindowWidth() - ImGui::GetContentRegionAvail().x;

			ImGui::Dummy(ImVec2(-distanceFromStart + ImGui::GetWindowWidth() * 0.5f - 38.0f, 0));

			Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();
			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

			const bool isPlaying = sceneManager.GetIsPlaying();
			const char* playIcon = isPlaying ? ICON_FA_PAUSE : ICON_FA_PLAY;

			if (isPlaying == true)
			{
				Simple::ImGuiEngine::SetEditorMode(Simple::eImGuiEditorMode::Playing);

				ECS::EntityComponentSystem& ecs = sceneManager.GetCurrentECS();
				const std::unordered_set<ECS::EntityID>& cameraEntityIDs = ecs.GetEntityIDsWithThisComponent<ECS::CameraComponent>();

				if (cameraEntityIDs.empty() == false)
				{
					ECS::CameraComponent* cameraComponent = ecs.GetEntity(*cameraEntityIDs.begin()).GetComponent<ECS::CameraComponent>();

					if (cameraComponent != nullptr)
					{
						graphicsEngine->SetCamera(&cameraComponent->camera);
					}
				}
			}
			else
			{
				Simple::ImGuiEngine::SetEditorMode(Simple::eImGuiEditorMode::Default);
				graphicsEngine->SetCamera(graphicsEngine->GetEditorCamera().get());
			}

			if (isPlaying == true)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImColor(1.0f, 0.0f, 0.0f, 1.0f).Value);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(0.6f, 0.0f, 0.0f, 1.0f).Value);
			}

			if (ImGui::Button(playIcon))
			{
				sceneManager.SetIsPlaying(!isPlaying);
			}

			if (isPlaying == true)
			{
				ImGui::PopStyleColor(2);
			}

			ImGui::EndMainMenuBar();
		}
	}
}
