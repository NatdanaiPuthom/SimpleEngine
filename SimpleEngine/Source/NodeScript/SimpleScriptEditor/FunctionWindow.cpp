#include "NodeScript/Precomplied/NodeScriptPch.hpp"
#include "FunctionWindow.h"
#include "VisualScriptingWindow.h"
#include "ScriptModifier.h"
#include "NodeTypeManager.h"
#include "Script.h"
#include "ScriptGlobal.h"

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
				SCRIPT::FunctionID createdFunctionID = SCRIPT::Modify::CreateGlobalFunction("Function");

				SCRIPT::Global::GetNodeTypeManager().GetFunction(createdFunctionID);
			}

			ImGui::Separator();

			std::vector<SCRIPT::FunctionView> functions = SCRIPT::Modify::GetFunctions();
			for (SCRIPT::FunctionView& functionView : functions)
			{

				char functionNameBuffer[32]{};
				strcpy_s(functionNameBuffer, functionView.GetName().c_str());
				if (ImGui::InputText("Name", functionNameBuffer, 32))
				{
					SCRIPT::Modify::SetFunctionName(functionView.GetID(), functionNameBuffer);
				}

				if (ImGui::Selectable(functionView.GetName().c_str()))
				{
					myParentWindow.SetNodeContext(functionView.GetNodeGraph(), nullptr);
				}

				if (ImGui::Button("Create Caller"))
				{
					SCRIPT::Modify::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, functionView.GetCallerNodeType().GetID());
				}

				if (ImGui::Button("Create Input"))
				{
					SCRIPT::Modify::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, functionView.GetInputNodeType().GetID());
				}

				if (ImGui::Button("Create Output"))
				{
					SCRIPT::Modify::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, functionView.GetOutputNodeType().GetID());
				}

				ImGui::Separator();
			}

		}
		ImGui::End();

	}

}
