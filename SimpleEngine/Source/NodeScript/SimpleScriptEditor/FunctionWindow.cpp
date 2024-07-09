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

			const std::vector<std::unique_ptr<SCRIPT::Function>>& functions = SCRIPT::NodeTypeManager::GetInstance().GetFunctions();
			for (SCRIPT::FunctionID functionID = 0; functionID < functions.size(); ++functionID)
			{
				SCRIPT::Function& function = *functions[functionID];

				char functionNameBuffer[32]{};
				strcpy_s(functionNameBuffer, function.GetName().c_str());
				if (ImGui::InputText("Name", functionNameBuffer, 32))
				{
					function.SetName(functionNameBuffer);
				}

				if (ImGui::Selectable(function.GetName().c_str()))
				{
					myParentWindow.SetNodeContext(function.GetNodeGraph(), nullptr);
				}

				if (ImGui::Button("Create Caller"))
				{
					SCRIPT::Modify::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, function.GetCallerNodeTypeID());
				}

				if (ImGui::Button("Create Input"))
				{
					SCRIPT::Modify::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, function.GetInputNodeTypeID());
				}

				if (ImGui::Button("Create Output"))
				{
					SCRIPT::Modify::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, function.GetOutputNodeTypeID());
				}

				ImGui::Separator();
			}

		}
		ImGui::End();

	}

}
