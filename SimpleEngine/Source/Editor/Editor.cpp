#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Editor.hpp"

#ifndef _SIMPLE
#include "Editor/Menu/MainMenuBar.hpp"
#include "Editor/Menu/HelpMenuBar.hpp"
#include "Editor/Menu/FileMenuBar.hpp"
#endif

namespace Editor
{
	bool EditorEngine::myStaticShouldRenderDebugLines = false;

#ifndef _SIMPLE
	EditorEngine::EditorEngine()
	{
		AddTool(std::move(std::make_unique<FileMenuBar>())); //TO-DO(11.0.6): implement save function to save current world to json so ecs can load from it
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