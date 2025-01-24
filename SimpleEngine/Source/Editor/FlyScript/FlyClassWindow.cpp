
#include "Editor/Precomplied/EditorPch.hpp"
#include "FlyClassWindow.hpp"
#include "NodeScript/Fly.hpp"
#include "NodeScriptingWindow.hpp"
#include "FlyScriptEditorUtilities.hpp"

namespace Editor
{

	FlyClassWindow::FlyClassWindow(NodeScriptingWindow& aParentWindow)
		: myParentWindow(aParentWindow)
	{
	}

	void FlyClassWindow::Draw()
	{
		if (ImGui::Begin("Class Window"))
		{
			DrawGraphSelection();
			ImGui::Separator();
			DrawFunctionSelection();
			ImGui::Separator();
			DrawVariables();

		}

		ImGui::End();
	}

	void FlyClassWindow::DrawGraphSelection()
	{
		Fly::ClassProxy currentClassProxy = myParentWindow.GetNodeContext().myClassProxy;
		if (ImGui::Selectable("Event Graph", currentClassProxy.GetEventGraph() == myParentWindow.GetNodeContext().myNodeGraphProxy))
		{
			myParentWindow.SetNodeContext(currentClassProxy.GetEventGraph(), currentClassProxy);
		}
	}

	void FlyClassWindow::DrawFunctionSelection()
	{
		Fly::ClassProxy currentClassProxy = myParentWindow.GetNodeContext().myClassProxy;

		if (ImGui::Button("Create Member Function"))
		{
			Fly::FunctionProxy createdFunctionProxy = currentClassProxy.CreateMemberFunction("Function1");
			myParentWindow.SetNodeContext(createdFunctionProxy.GetNodeGraph(), currentClassProxy);
			myParentWindow.SetSelectedFunctionProxy(createdFunctionProxy);
			ImGui::SetNextItemOpen(true);
		}


		if (ImGui::TreeNode("Member Functions"))
		{

			std::vector<Fly::FunctionProxy> memberFunctions = currentClassProxy.GetFunctions();

			for (Fly::FunctionProxy& memberFunctionProxy : memberFunctions)
			{

				if (ImGui::Selectable(memberFunctionProxy.GetName().c_str(), memberFunctionProxy.GetNodeGraph() == myParentWindow.GetNodeContext().myNodeGraphProxy))
				{
					myParentWindow.SetNodeContext(memberFunctionProxy.GetNodeGraph(), myParentWindow.GetNodeContext().myClassProxy);
					myParentWindow.SetSelectedFunctionProxy(memberFunctionProxy);
					break;
				}
			}

			ImGui::TreePop();
		}

	}



	void FlyClassWindow::DrawVariables()
	{
		Fly::ClassProxy currentClass = myParentWindow.GetNodeContext().myClassProxy;

		if (ImGui::Button("Create Variable"))
		{
			myParentWindow.GetNodeContext().myClassProxy.CreateVariable(Fly::GenericDataTypeProxy(Fly::GetDataTypeID<bool>()), "Var", nullptr);
			ImGui::SetNextItemOpen(true);
		}

		if (ImGui::TreeNode("Variables"))
		{

			ImGui::Separator();

			std::vector<Fly::VariableProxy> variableProxys = currentClass.GetVariables();
			for (Fly::VariableProxy& variableProxy : variableProxys)
			{
				const std::string treeNodeStrID = std::string("##ScriptVariable" + std::to_string(variableProxy.GetID()));
				if (ImGui::TreeNode(treeNodeStrID.c_str(), std::string(variableProxy.GetName()).c_str()))
				{
					ImGui::SameLine(ImGui::GetWindowWidth() - 30.f);

					ImGui::BeginDisabled();
					ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(Fly::GenericDataTypeProxy(variableProxy.GetDataTypeID()).GetColor())));
					ImGui::EndDisabled();

					ModifyVariablePopup(variableProxy);

					ImGui::TreePop();
				}
				else
				{
					ImGui::SameLine(ImGui::GetWindowWidth() - 30.f);

					ImGui::BeginDisabled();
					ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(Fly::GenericDataTypeProxy(variableProxy.GetDataTypeID()).GetColor())));
					ImGui::EndDisabled();
				}

				ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 1));
				ImGui::Separator();
				ImGui::PopStyleColor();
			}

			ImGui::TreePop();
		}

	}

	void FlyClassWindow::ModifyVariablePopup(Fly::VariableProxy& aVariableProxy)
	{
		std::string variableName(aVariableProxy.GetName());

		if (ImGui::InputString<35>("##VariableName", variableName))
		{
			aVariableProxy.SetName(variableName, nullptr);
		}

		Fly::GenericDataTypeProxy currentDataTypeProxy(aVariableProxy.GetDataTypeID());

		ImGui::Separator();
		if (DataTypeComboEditableFilter("##ChangeDataType", currentDataTypeProxy))
		{
			aVariableProxy.SetDataType(currentDataTypeProxy, nullptr);
		}

		ImGui::Text("Default Value:");
		ImGui::SameLine();
		aVariableProxy.ViewAndEditDefaultValue(nullptr);

		ImGui::Separator();

		if (ImGui::Button("Create Getter"))
		{
			//myParentWindow.GetNodeContext().myClassProxy.GetEventGraphProxy().CreateGetterNode(aVariableProxy, Fly::Vec2{}, &myParentWindow.GetCommandTracker());
			ImGui::CloseCurrentPopup();

		}

		ImGui::SameLine();

		if (ImGui::Button("Create Setter"))
		{
			//myParentWindow.GetNodeContext().myClassProxy.GetEventGraphProxy().CreateSetterNode(aVariableProxy, Fly::Vec2{}, &myParentWindow.GetCommandTracker());
		}

		if (ImGui::Button("Delete Variable"))
		{
			aVariableProxy.Destroy(nullptr);
		}
	}

}