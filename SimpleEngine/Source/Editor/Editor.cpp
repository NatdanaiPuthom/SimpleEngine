#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Editor.hpp"

#ifndef _SIMPLE
#include "Graphics/Renderer/Renderer.hpp"
#include "Editor/Menu/MainMenuBar.hpp"
#endif

namespace Simple
{
#ifndef _SIMPLE
	Editor::Editor()
	{
		AddTool(std::move(std::make_unique<Tool::MainMenuBar>()));
	}

	Editor::~Editor()
	{
	}

	void Editor::Init()
	{
		for (const auto& tool : myTools)
		{
			tool->Init();
		}
	}

	void Editor::Update()
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
	}

	void Editor::Render()
	{
		for (const std::unique_ptr<Simple::ToolInterface>& tool : myTools)
		{
			tool->Draw();
		}
	}

	void Editor::AddTool(std::unique_ptr<Simple::ToolInterface> aTool)
	{
		myTools.push_back(std::move(aTool));
	}

#else
	Editor::Editor() {}
	Editor::~Editor() {}

	void Editor::Init() {}
	void Editor::Update() {}
	void Editor::Render() {}

	void Editor::AddTool(std::unique_ptr<Simple::ToolInterface> /*aTool*/) {}
#endif
}