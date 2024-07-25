#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Editor.hpp"

#ifndef _SIMPLE
#include "Editor/Menu/MainMenuBar.hpp"
#include "Editor/Menu/HelpMenuBar.hpp"
#include "Editor/Menu/SceneMenuBar.hpp"
#include "Editor/Menu/PlayMenuBar.hpp"
#include "MainSingleton/MainSingleton.hpp"
#endif

namespace Editor
{
	size_t EditorEngine::mySelectedEntityID = static_cast<size_t>(-1);

#ifndef _SIMPLE
	EditorEngine::EditorEngine()
	{
		AddTool(std::make_unique<SceneMenuBar>());
		AddTool(std::make_unique<MainMenuBar>());
		AddTool(std::make_unique<HelpMenuBar>());
		AddTool(std::make_unique<PlayMenuBar>());
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
		if (MainSingleton::GetSceneManager().GetIsPlaying() == false)
		{
			Global::GetGraphicsEngine()->GetEditorCamera()->Update(Global::GetDeltaTime(), Global::GetEngineHWND());
		}

		for (const std::unique_ptr<ToolInterface>& tool : myTools)
		{
			if (tool->myIsActive == true)
			{
				tool->Update();
			}
		}
	}

	void EditorEngine::Render()
	{
		for (const std::unique_ptr<ToolInterface>& tool : myTools)
		{
			if (tool->myIsActive == true)
			{
				tool->Draw();
			}
		}
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