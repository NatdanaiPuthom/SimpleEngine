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
				SCRIPT::ScriptModifier::CreateFunction("Function");
			}

			const std::vector<SCRIPT::Function*>& functions = SCRIPT::NodeTypeManager::GetFunctions();
			for (SCRIPT::FunctionID functionID = 0; functionID < functions.size(); ++functionID)
			{
				SCRIPT::Function* function = functions[functionID];

				if (ImGui::Button("Create Caller"))
				{
					myParentWindow.GetCurrentContext().script->GetModifier().CreateNode(function->GetCallerNodeTypeID());
				}

				if (ImGui::Button("Create Input"))
				{
					myParentWindow.GetCurrentContext().script->GetModifier().CreateNode(function->GetInputNodeTypeID());
				}

				if (ImGui::Button("Create Output"))
				{
					myParentWindow.GetCurrentContext().script->GetModifier().CreateNode(function->GetOutputNodeTypeID());
				}

				ImGui::Separator();
			}

		}
		ImGui::End();

	}

}
