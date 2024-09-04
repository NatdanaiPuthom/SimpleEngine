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
		Fly::ClassView currentClass = myParentWindow.GetNodeContext().myClassView;

		if (ImGui::Begin("VariableWindow"))
		{
			if (ImGui::Button("Create Variable"))
			{
				currentClass.CreateVariable(Fly::DataTypeView(Fly::GetDataTypeID<bool>()), nullptr);
			}

			ImGui::Separator();

			const std::vector<Fly::VariableView> variables = currentClass.GetVariables();
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

	void VariableWindow::ModifyVariablePopup(Fly::VariableView aVariableView)
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

		ImGui::Text("Default value:");
		ImGui::SameLine();
	
		aVariableView.EditDefaultValue(nullptr);

		ImGui::Separator();

		if (ImGui::Button("Create Getter"))
		{
			myParentWindow.GetNodeContext().myClassView.GetEventGraphView().CreateGetterNode(aVariableView, Fly::Vec2{}, &myParentWindow.GetCommandTracker());
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