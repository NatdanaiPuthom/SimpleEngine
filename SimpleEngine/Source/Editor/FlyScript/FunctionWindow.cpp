#include "Editor/Precomplied/EditorPch.hpp"
#include "FunctionWindow.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/SimpleScript/Core/ScriptModifier.hpp"
#include "NodeScript/SimpleScript/Core/FlyClass.hpp"
#include "NodeScript/SimpleScript/Core/Global/ScriptGlobal.hpp"

namespace Editor
{

	FunctionWindow::FunctionWindow(NodeScriptingWindow& aParentWindow)
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
				Fly::CreateGlobalFunction("Function");
			}

			ImGui::Separator();

			std::vector<Fly::FunctionView> functions = Fly::GetFunctions();
			for (Fly::FunctionView& functionView : functions)
			{

				char functionNameBuffer[32]{};
				strcpy_s(functionNameBuffer, functionView.GetName().c_str());
				if (ImGui::InputText("Name", functionNameBuffer, 32))
				{
					Fly::SetFunctionName(functionView.GetID(), functionNameBuffer);
				}

				const std::string functionlabel = functionView.GetName() + "##function" + std::to_string(functionView.GetID());
				if (ImGui::Selectable(functionlabel.c_str()))
				{
					myParentWindow.SetNodeContext(functionView.GetNodeGraph(), nullptr);
				}

				if (ImGui::Button("Create Caller"))
				{
					Fly::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, functionView.GetCallerNodeType().GetID());
				}

				if (ImGui::Button("Create Input"))
				{
					Fly::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, functionView.GetInputNodeType().GetID());
				}

				if (ImGui::Button("Create Output"))
				{
					Fly::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, functionView.GetOutputNodeType().GetID());
				}

				ImGui::Separator();
			}

		}
		ImGui::End();

	}

}
