#include "Editor/Precomplied/EditorPch.hpp"
#include "FunctionWindow.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/SimpleScript/Core/Fly.hpp"
#include "FlyScriptEditorUtilities.hpp"

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
					Fly::SetFunctionName(functionView, functionNameBuffer);
				}

				const std::string functionlabel = functionView.GetName() + "##function" + std::to_string(functionView.GetID());
				if (ImGui::Selectable(functionlabel.c_str()))
				{
					myParentWindow.SetNodeContext(functionView.GetNodeGraph(), Fly::ClassView());
				}

				if (ImGui::Button("Create Caller"))
				{
					Fly::CreateNode(myParentWindow.GetNodeContext().nodeGraphView, functionView.GetCallerNodeType());
				}
			}

		}
		ImGui::End();

	}

}
