
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
		Fly::ClassFacade currentClassFacade = myParentWindow.GetNodeContext().myClassFacade;
		if (ImGui::Selectable("Event Graph", currentClassFacade.GetEventGraphFacade() == myParentWindow.GetNodeContext().myNodeGraphFacade))
		{
			myParentWindow.SetNodeContext(currentClassFacade.GetEventGraphFacade(), currentClassFacade);
		}
	}

	void FlyClassWindow::DrawFunctionSelection()
	{
		Fly::ClassFacade currentClassFacade = myParentWindow.GetNodeContext().myClassFacade;

		if (ImGui::Button("Create Member Function"))
		{
			Fly::FunctionFacade createdFunctionFacade = currentClassFacade.CreateMemberFunction("Function1");
			myParentWindow.SetNodeContext(createdFunctionFacade.GetNodeGraphFacade(), currentClassFacade);
			myParentWindow.SetSelectedFunctionFacade(createdFunctionFacade);
			ImGui::SetNextItemOpen(true);
		}


		if (ImGui::TreeNode("Member Functions"))
		{

			std::vector<Fly::FunctionFacade> memberFunctions = currentClassFacade.GetFunctions();

			for (Fly::FunctionFacade& memberFunctionFacade : memberFunctions)
			{

				if (ImGui::Selectable(memberFunctionFacade.GetName().c_str(), memberFunctionFacade.GetNodeGraphFacade() == myParentWindow.GetNodeContext().myNodeGraphFacade))
				{
					myParentWindow.SetNodeContext(memberFunctionFacade.GetNodeGraphFacade(), myParentWindow.GetNodeContext().myClassFacade);
					myParentWindow.SetSelectedFunctionFacade(memberFunctionFacade);
					break;
				}
			}

			ImGui::TreePop();
		}

	}



	void FlyClassWindow::DrawVariables()
	{
		Fly::ClassFacade currentClass = myParentWindow.GetNodeContext().myClassFacade;

		if (ImGui::Button("Create Variable"))
		{
			myParentWindow.GetNodeContext().myClassFacade.CreateVariable(Fly::DataTypeFacade(Fly::GetDataTypeID<bool>()), nullptr);
			ImGui::SetNextItemOpen(true);
		}

		if (ImGui::TreeNode("Variables"))
		{

			ImGui::Separator();

			std::vector<Fly::VariableFacade> variableFacades = Fly::GetVariables(currentClass);
			for (Fly::VariableFacade& variableFacade : variableFacades)
			{
				const std::string treeNodeStrID = std::string("##ScriptVariable" + std::to_string(variableFacade.GetID()));
				if (ImGui::TreeNode(treeNodeStrID.c_str(), variableFacade.GetName().c_str()))
				{
					ImGui::SameLine(ImGui::GetWindowWidth() - 30.f);

					ImGui::BeginDisabled();
					ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(variableFacade.GetDataType().GetColor())));
					ImGui::EndDisabled();

					ModifyVariablePopup(variableFacade);

					ImGui::TreePop();
				}
				else
				{
					ImGui::SameLine(ImGui::GetWindowWidth() - 30.f);

					ImGui::BeginDisabled();
					ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(variableFacade.GetDataType().GetColor())));
					ImGui::EndDisabled();
				}

				ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 1));
				ImGui::Separator();
				ImGui::PopStyleColor();
			}

			ImGui::TreePop();
		}

	}

	void FlyClassWindow::ModifyVariablePopup(Fly::VariableFacade& aVariableFacade)
	{
		char buffer[35]{};
		strcpy_s(buffer, aVariableFacade.GetName().c_str());

		if (ImGui::InputText("##VariableName", buffer, IM_ARRAYSIZE(buffer)))
		{
			aVariableFacade.SetName(buffer, nullptr);
		}

		Fly::DataTypeFacade currentDataTypeFacade = aVariableFacade.GetDataType();

		ImGui::Separator();
		if (DataTypeComboEditableFilter("##ChangeDataType", currentDataTypeFacade))
		{
			aVariableFacade.SetDataType(currentDataTypeFacade, nullptr);
		}

		ImGui::Text("Default Value:");
		ImGui::SameLine();
		aVariableFacade.EditDefaultValue(nullptr);

		ImGui::Separator();

		if (ImGui::Button("Create Getter"))
		{
			myParentWindow.GetNodeContext().myClassFacade.GetEventGraphFacade().CreateGetterNode(aVariableFacade, Fly::Vec2{}, &myParentWindow.GetCommandTracker());
			ImGui::CloseCurrentPopup();

		}

		ImGui::SameLine();

		if (ImGui::Button("Create Setter"))
		{
			myParentWindow.GetNodeContext().myClassFacade.GetEventGraphFacade().CreateSetterNode(aVariableFacade, Fly::Vec2{}, &myParentWindow.GetCommandTracker());
		}

		if (ImGui::Button("Delete Variable"))
		{
			aVariableFacade.Destroy(nullptr);
		}
	}

}