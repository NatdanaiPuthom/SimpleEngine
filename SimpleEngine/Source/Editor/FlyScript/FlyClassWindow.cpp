
#include "Editor/Precomplied/EditorPch.hpp"
#include "FlyClassWindow.hpp"
#include "NodeScript/SimpleScript/Core/Fly.hpp"
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
		Fly::ClassView currentClassView = myParentWindow.GetNodeContext().myClassView;
		if (ImGui::Selectable("Event Graph", currentClassView.GetEventGraphView() == myParentWindow.GetNodeContext().myNodeGraphView))
		{
			myParentWindow.SetNodeContext(currentClassView.GetEventGraphView(), currentClassView);
		}
	}

	void FlyClassWindow::DrawFunctionSelection()
	{
		Fly::ClassView currentClassView = myParentWindow.GetNodeContext().myClassView;

		if (ImGui::Button("Create Member Function"))
		{
			Fly::FunctionView createdFunctionView = currentClassView.CreateMemberFunction("Function1");
			myParentWindow.SetNodeContext(createdFunctionView.GetNodeGraphView(), currentClassView);
			myParentWindow.SetSelectedFunctionView(createdFunctionView);
			ImGui::SetNextItemOpen(true);
		}


		if (ImGui::TreeNode("Member Functions"))
		{

			std::vector<Fly::FunctionView> memberFunctions = currentClassView.GetFunctions();

			for (Fly::FunctionView& memberFunctionView : memberFunctions)
			{

				if (ImGui::Selectable(memberFunctionView.GetName().c_str(), memberFunctionView.GetNodeGraphView() == myParentWindow.GetNodeContext().myNodeGraphView))
				{
					myParentWindow.SetNodeContext(memberFunctionView.GetNodeGraphView(), myParentWindow.GetNodeContext().myClassView);
					myParentWindow.SetSelectedFunctionView(memberFunctionView);
					break;
				}
			}

			ImGui::TreePop();
		}

	}



	void FlyClassWindow::DrawVariables()
	{
		Fly::ClassView currentClass = myParentWindow.GetNodeContext().myClassView;

		if (ImGui::Button("Create Variable"))
		{
			myParentWindow.GetNodeContext().myClassView.CreateVariable(Fly::DataTypeView(Fly::GetDataTypeID<bool>()), nullptr);
			ImGui::SetNextItemOpen(true);
		}

		if (ImGui::TreeNode("Variables"))
		{

			ImGui::Separator();

			std::vector<Fly::VariableView> variables = Fly::GetVariables(currentClass);
			for (Fly::VariableView& variable : variables)
			{
				const std::string treeNodeStrID = std::string("##ScriptVariable" + std::to_string(variable.GetID()));
				if (ImGui::TreeNode(treeNodeStrID.c_str(), variable.GetName().c_str()))
				{
					ImGui::SameLine(ImGui::GetWindowWidth() - 30.f);

					ImGui::BeginDisabled();
					ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(variable.GetDataType().GetColor())));
					ImGui::EndDisabled();

					ModifyVariablePopup(variable);

					ImGui::TreePop();
				}
				else
				{
					ImGui::SameLine(ImGui::GetWindowWidth() - 30.f);

					ImGui::BeginDisabled();
					ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(variable.GetDataType().GetColor())));
					ImGui::EndDisabled();
				}

				ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 1));
				ImGui::Separator();
				ImGui::PopStyleColor();
			}

			ImGui::TreePop();
		}

	}

	void FlyClassWindow::ModifyVariablePopup(Fly::VariableView& aVariableView)
	{
		char buffer[35]{};
		strcpy_s(buffer, aVariableView.GetName().c_str());

		if (ImGui::InputText("##VariableName", buffer, IM_ARRAYSIZE(buffer)))
		{
			aVariableView.SetName(buffer, nullptr);
		}

		Fly::DataTypeView currentDataTypeView = aVariableView.GetDataType();

		ImGui::Separator();
		if (DataTypeComboEditableFilter("##ChangeDataType", currentDataTypeView))
		{
			aVariableView.SetDataType(currentDataTypeView, nullptr);
		}

		ImGui::Text("Default Value:");
		ImGui::SameLine();
		aVariableView.EditDefaultValue(nullptr);

		ImGui::Separator();

		if (ImGui::Button("Create Getter"))
		{
			myParentWindow.GetNodeContext().myClassView.GetEventGraphView().CreateGetterNode(aVariableView, Fly::Vec2{}, &myParentWindow.GetCommandTracker());
			ImGui::CloseCurrentPopup();

		}

		ImGui::SameLine();

		if (ImGui::Button("Create Setter"))
		{
			myParentWindow.GetNodeContext().myClassView.GetEventGraphView().CreateSetterNode(aVariableView, Fly::Vec2{}, &myParentWindow.GetCommandTracker());
		}

		if (ImGui::Button("Delete Variable"))
		{
			aVariableView.Destroy(nullptr);
		}
	}

}