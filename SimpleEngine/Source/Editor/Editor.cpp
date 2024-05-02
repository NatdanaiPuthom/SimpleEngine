#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Editor.hpp"

#ifndef _SIMPLE
#include "Graphics/Renderer/Renderer.hpp"
#include "Editor/Menu/MainMenuBar.hpp"
#endif

namespace Editor
{
#ifndef _SIMPLE
	EditorEngine::EditorEngine()
	{
		AddTool(std::move(std::make_unique<MainMenuBar>()));
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
		if (SimpleUtilities::InputManager::GetInstance().IsKeyPressed(VK_F5))
		{
			if (Global::IsFullScreen())
			{
				Global::SetWindowSize({ 1280, 720 }, false);
			}
			else
			{
				Global::SetWindowSize({ 0, 0 }, true);
			}
		}

		for (const std::unique_ptr<ToolInterface>& tool : myTools)
		{
			tool->Update();
		}
	}

	void EditorEngine::Render()
	{
		for (const std::unique_ptr<ToolInterface>& tool : myTools)
		{
			tool->Draw();
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