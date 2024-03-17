#include "Editor/Precomplied/EditorPch.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Menu/MainMenuBar.hpp"

namespace Simple
{
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
}