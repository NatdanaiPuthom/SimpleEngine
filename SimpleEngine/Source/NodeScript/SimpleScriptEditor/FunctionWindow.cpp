#include "NodeScript/Precomplied/NodeScriptPch.hpp"
#include "FunctionWindow.h"
#include "VisualScriptingWindow.h"
#include "ScriptModifier.h"
#include "NodeTypeManager.h"
#include "Script.h"

namespace Editor
{

	FunctionWindow::FunctionWindow(VisualScriptingWindow& aParentWindow)
		: myParentWindow(aParentWindow)
	{
	}

	FunctionWindow::~FunctionWindow()
	{
	}

	void FunctionWindow::Update()
	{

		if (ImGui::Begin("Global Functions"))
		{

			if (ImGui::Button("Create Function"))
			{
				SCRIPT::Modify::CreateGlobalFunction("Function");
				//SCRIPT::ScriptModifier::CreateFunction("Function");
			}

			const std::vector<std::unique_ptr<SCRIPT::Function>>& functions = SCRIPT::NodeTypeManager::GetInstance().GetFunctions();
			for (SCRIPT::FunctionID functionID = 0; functionID < functions.size(); ++functionID)
			{
				SCRIPT::Function* function = functions[functionID].get();

				if (ImGui::Button("Create Caller"))
				{
					SCRIPT::Modify::CreateNode(*myParentWindow.GetCurrentContext().nodeGraph, function->GetCallerNodeTypeID());
				}

				if (ImGui::Button("Create Input"))
				{
					SCRIPT::Modify::CreateNode(*myParentWindow.GetCurrentContext().nodeGraph, function->GetInputNodeTypeID());
				}

				if (ImGui::Button("Create Output"))
				{
					SCRIPT::Modify::CreateNode(*myParentWindow.GetCurrentContext().nodeGraph, function->GetOutputNodeTypeID());
				}

				ImGui::Separator();
			}

		}
		ImGui::End();

	}

}
