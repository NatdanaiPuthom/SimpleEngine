#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Editor.hpp"

#include "MainSingleton/MainSingleton.hpp"

#include "Editor/Temp/Build.hpp"
#include "Editor/Temp/Files.hpp"
#include "Editor/Temp/Settings.hpp"
#include "Editor/Temp/Audio.hpp"

namespace Editor
{
	size_t EditorEngine::mySelectedEntityID = static_cast<size_t>(-1);

	EditorEngine::EditorEngine()
	{
		/*AddTool(std::make_unique<BuildMenuBar>());
		AddTool(std::make_unique<SceneMenuBar>());
		AddTool(std::make_unique<MainMenuBar>());
		AddTool(std::make_unique<HelpMenuBar>());
		AddTool(std::make_unique<SettingsMenuBar>());
		AddTool(std::make_unique<PlayMenuBar>());*/
	}

	EditorEngine::~EditorEngine()
	{
	}

	void EditorEngine::Init()
	{
		AddPopUpWindow<Build>();
		AddMenuTab<Files>();

		std::shared_ptr<Settings> settings = AddMenuTab<Settings>();
		settings->AddChildren<Audio>();
	}

	void EditorEngine::Update()
	{
		/*if (MainSingleton::GetSceneManager().GetIsPlaying() == false)
		{
			Global::GetGraphicsEngine()->GetEditorCamera()->Update(Global::GetDeltaTime(), Global::GetEngineHWND());
		}

		for (const std::unique_ptr<ToolInterface>& tool : myTools)
		{
			if (tool->myIsActive == true)
			{
				tool->Update();
			}
		}*/
	}

	void EditorEngine::Render()
	{
		for (const std::shared_ptr<PopUp> window : myPopUpWindows)
		{
			window->Update();
		}

		for (const std::shared_ptr<MainMenuTab> menuTab : myMainMenuTabs)
		{
			if (menuTab->IsActive())
			{
				menuTab->Render();
			}
		}

		for (const std::shared_ptr<PopUp> window : myPopUpWindows)
		{
			if (window->IsActive())
			{
				window->Render();
			}
		}

		//for (const std::unique_ptr<ToolInterface>& tool : myTools)
		//{
		//	if (tool->myIsActive == true)
		//	{
		//		tool->Draw();
		//	}
		//}

		//{	//Render Orientation Cube  
		//	//TO-DO(v11.4.4): Make own class for this
  //          const Graphics::Camera* camera = Global::GetGraphicsEngine()->GetCurrentCamera();  
  //          Math::Matrix4x4f view = camera->GetViewMatrix();  
  //          ImVec2 windowPos = ImGui::GetWindowPos();  
  //          ImGuizmo::ViewManipulate(&view(1, 1), 16, ImVec2(windowPos.x + 775, windowPos.y + 375), ImVec2(64, 64), 0x00000000);
		//}
	}
}
