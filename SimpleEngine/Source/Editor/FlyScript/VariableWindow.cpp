#include "Editor/Precomplied/EditorPch.hpp"
#include "VariableWindow.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/SimpleScript/Core/Fly.hpp"
#include "NodeScript/SimpleScript/Core/Global/FlyGlobal.hpp"
#include "FlyScriptEditorUtilities.hpp"

namespace Editor
{

	VariableWindow::VariableWindow(NodeScriptingWindow& aParentWindow)
		: myParentWindow(aParentWindow)
	{
	}

	void VariableWindow::Update()
	{
		Fly::ClassView currentClass = myParentWindow.GetNodeContext().classView;

		if (ImGui::Begin("VariableWindow"))
		{
			if (ImGui::Button("Create Variable"))
			{
				
				Fly::CreateVariable(myParentWindow.GetNodeContext().classView, Fly::DataTypeView(Fly::GetDataTypeID<bool>()));
			}

			ImGui::Separator();

			std::vector<Fly::VariableView> variables = Fly::GetVariables(currentClass);
			for (const Fly::VariableView& variable : variables)
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

		}

		ImGui::End();
	}

	void VariableWindow::ModifyVariablePopup(const Fly::VariableView& aVariableView)
	{
		char buffer[35]{};
		strcpy_s(buffer, aVariableView.GetName().c_str());

		if (ImGui::InputText("##VariableName", buffer, IM_ARRAYSIZE(buffer)))
		{
			Fly::SetVariableName(aVariableView, buffer, myParentWindow.GetNodeContext().classView);
		}

		Fly::DataTypeView currentDataType = aVariableView.GetDataType();

		ImGui::Separator();
		if (DataTypeComboEditableFilter("##ChangeDataType", currentDataType))
		{
			SetVariableDataType(aVariableView, currentDataType, myParentWindow.GetNodeContext().classView, nullptr);
		}

		ImGui::Text("Default value:");
		ImGui::SameLine();
		EditVariableDefaultValue(aVariableView, myParentWindow.GetNodeContext().classView, nullptr);

		ImGui::Separator();

		if (ImGui::Button("Create Getter"))
		{
			CreateGetterNode(myParentWindow.GetNodeContext().classView, myParentWindow.GetNodeContext().nodeGraphView, aVariableView);
			ImGui::CloseCurrentPopup();

		}

		ImGui::SameLine();

		if (ImGui::Button("Create Setter"))
		{
			CreateSetterNode(myParentWindow.GetNodeContext().classView, myParentWindow.GetNodeContext().nodeGraphView, aVariableView);
		}

		if (ImGui::Button("Delete Variable"))
		{
			DestroyVariable(aVariableView, myParentWindow.GetNodeContext().classView, nullptr);
		}
	}
}