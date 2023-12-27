#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/ImGuiManager/Tools/Debug.hpp"

DebugTool::DebugTool()
{
}

DebugTool::~DebugTool()
{
}

void DebugTool::Draw()
{
	if (ImGui::Begin("Debug"))
	{
		auto renderer = SimpleGlobal::GetRenderer();

		bool debugMode = renderer->IsDebugModeOn();
		if (ImGui::Checkbox("Draw BoundingBox", &debugMode))
		{
			renderer->SetDebugMode(debugMode);
		}
	}
	ImGui::End();
}
