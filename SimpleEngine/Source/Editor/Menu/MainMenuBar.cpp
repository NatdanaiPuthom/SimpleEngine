#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/MainMenuBar.hpp"
#include "Editor/Windows/SettingsWindow.hpp"
#include "Editor/Windows/HierarchyWindow.hpp"
#include "Editor/Windows/AssetWindow.hpp"
#include "Editor/Windows/DeferredSceneWindow.hpp"
#include "Editor/Windows/PostProcessingWindow.hpp"
#include "Editor/FlyScript/NodeScriptingWindow.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include "External/dearimgui/imguizmo/ImGuizmo.h"
#include "Editor/Editor.hpp"

namespace Editor
{

	MainMenuBar::MainMenuBar()
	{
	}

	MainMenuBar::~MainMenuBar()
	{
	}

	bool ActiveWindowData::IsWindowActive(eWindowType aWindowType) const
	{
		return myIsWindowActive[static_cast<size_t>(aWindowType)];
	}

	void ActiveWindowData::SetActiveWindow(eWindowType aWindowType, const bool aIsActive)
	{
		assert(aWindowType != eWindowType::Count);

		for (bool& isActive : myIsWindowActive)
		{
			isActive = false;
		}

		myIsWindowActive[static_cast<size_t>(aWindowType)] = aIsActive;
	}

	void MainMenuBar::Init()
	{
		mySettingsWindow = std::make_unique<SettingsWindow>();
		myAssetWindow = std::make_unique<AssetWindow>();
		myHierarchyWindow = std::make_unique<HierarchyWindow>();
		myDeferredSceneWindow = std::make_unique<DeferredSceneWindow>();
		myPostProcessWindow = std::make_unique<PostProcessingWindow>();
		myNodeScriptingWindow = std::make_unique<NodeScriptingWindow>();

		LoadSettingsFromJson();

		mySettingsWindow->Init();
		myAssetWindow->Init();
		myHierarchyWindow->Init();
		myDeferredSceneWindow->Init();
		myPostProcessWindow->Init();
		myNodeScriptingWindow->Init();

		myAssetWindow->myActiveWindowData = &myActiveWindowData;
		myAssetWindow->myNodeScriptingWindow = myNodeScriptingWindow.get();
	}

	void MainMenuBar::Update()
	{
		Simpleton::InputManager& inputManager = MainSingleton::GetInputManager();

		static const char* const windowNames[] = { "Editor", "Deferred", "PostProcess", "NodeScript" };
		static const char* const keyShortCuts[] = { "F1", "F2", "F3", "F4" };

		for (unsigned int i = 0; i < static_cast<unsigned int>(eWindowType::Count); ++i)
		{
			if (inputManager.IsKeyPressed(VK_F1 + i))
			{
				myActiveWindowData.SetActiveWindow(static_cast<eWindowType>(i), !myActiveWindowData.IsWindowActive(static_cast<eWindowType>(i)));
			}
		}

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

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Windows"))
			{
				for (unsigned int i = 0; i < static_cast<unsigned int>(eWindowType::Count); ++i)
				{
					if (ImGui::MenuItem(windowNames[i], keyShortCuts[i], myActiveWindowData.IsWindowActive(static_cast<eWindowType>(i))))
					{
						myActiveWindowData.SetActiveWindow(static_cast<eWindowType>(i), !myActiveWindowData.IsWindowActive(static_cast<eWindowType>(i)));
					}
				}

				MenuItemFullScreen();

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (myActiveWindowData.IsWindowActive(eWindowType::Editor))
		{
			mySettingsWindow->Update();
			myHierarchyWindow->Update();
			myAssetWindow->Update();
		}
	}

	void MainMenuBar::Draw()
	{
		if (myActiveWindowData.IsWindowActive(eWindowType::Editor))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 3));

			if (ImGui::Begin("Game##MainMenuBar", 0, ImGuiWindowFlags_NoScrollbar))
			{
				Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
				const Graphics::eRasterizerState currentRasterizerState = graphicsEngine->GetCurrentRasterizerState();

				ImTextureID textureID = graphicsEngine->GetShaderResourceView(Graphics::eRenderTargetType::PostProcessing).Get();

				if (currentRasterizerState != Graphics::eRasterizerState::BackfaceCulling
					&& currentRasterizerState != Graphics::eRasterizerState::NoFaceCulling)
				{
					textureID = graphicsEngine->GetShaderResourceView(Graphics::eRenderTargetType::Deferred).Get();
				}

				const ImVec2 size = ImGui::GetContentRegionAvail();
				ImGui::Image(textureID, size);

				if (EditorEngine::mySelectedEntityID != static_cast<size_t>(-1)) //TO-DO(v11.4.1): This shouldn't be here pls fix, future me
				{
					const ImVec2 topLeft = ImGui::GetItemRectMin();
					const ImVec2 bottomRight = ImGui::GetItemRectMax();
					ImGuizmo::SetOrthographic(false);
					ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
					ImGuizmo::SetRect(topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);

					ECS::Entity& selectedEntity = MainSingleton::GetSceneManager().GetCurrentECS().GetEntity(EditorEngine::mySelectedEntityID);
					ECS::TransformComponent* transformComponent = selectedEntity.GetComponent<ECS::TransformComponent>();

					if (transformComponent != nullptr)
					{
						const Graphics::Camera* camera = Global::GetGraphicsEngine()->GetCurrentCamera();

						Math::Matrix4x4f objectMatrix = transformComponent->transform.GetMatrix();
						const Math::Matrix4x4f view = camera->GetViewMatrix();
						const Math::Matrix4x4f proj = camera->GetProjectionMatrix();

						static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;

						if (MainSingleton::GetInputManager().IsKeyPressed('T'))
						{
							operation = ImGuizmo::OPERATION::TRANSLATE;
						}
						else if (MainSingleton::GetInputManager().IsKeyPressed('R'))
						{
							operation = ImGuizmo::OPERATION::ROTATE;
						}
						else if (MainSingleton::GetInputManager().IsKeyPressed('S') && !MainSingleton::GetInputManager().GetMouseIsHidden())
						{
							operation = ImGuizmo::OPERATION::SCALE;
						}

						if (ImGuizmo::Manipulate(&view(1, 1),
							&proj(1, 1),
							operation,
							ImGuizmo::MODE::WORLD,
							&objectMatrix(1, 1)
						))
						{
							switch (operation)
							{
							case ImGuizmo::OPERATION::TRANSLATE:
								transformComponent->transform.SetPosition(objectMatrix.GetPosition());
								break;
							case ImGuizmo::OPERATION::ROTATE:
								//TO-DO(v11.4.1): Fix Quaternion and Rotation pls!!! It has been forever!
								break;
							case ImGuizmo::OPERATION::SCALE:
								transformComponent->transform.SetScale(objectMatrix.GetScale());
								break;
							default:
								break;
							}
						}
					}
				}
			}
			ImGui::End();

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
		}

		DrawTools();
	}

	void MainMenuBar::MenuItemFullScreen()
	{
		bool isFullScreen = Global::IsFullScreen();

		if (ImGui::MenuItem("FullScreen", "F5", &isFullScreen))
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
	}

	void MainMenuBar::DrawTools()
	{
		if (myActiveWindowData.IsWindowActive(eWindowType::Editor))
		{
			mySettingsWindow->Draw();
			myAssetWindow->Draw();
			myHierarchyWindow->Draw(); //TO-DO(v9.31.1): For now HierachyWindow should always be run last due to removing Entities during run time. Fix/Look into it in future
		}

		if (myActiveWindowData.IsWindowActive(eWindowType::Deferred))
		{
			myDeferredSceneWindow->Draw();
		}

		if (myActiveWindowData.IsWindowActive(eWindowType::PostProcess))
		{
			myPostProcessWindow->Draw();
		}

		if (myActiveWindowData.IsWindowActive(eWindowType::NodeScript))
		{
			myNodeScriptingWindow->Draw();
		}
	}

	void MainMenuBar::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_EDITOR);

		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		const nlohmann::json settings = json["Editor_Settings"];

		myActiveWindowData.SetActiveWindow(eWindowType::Editor, settings["Editor"]["Active"]);
		myHierarchyWindow->myIsActive = settings["Windows"]["Show_Inspector"];
	}
}