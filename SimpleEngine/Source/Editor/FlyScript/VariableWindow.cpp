#include "Editor/Precomplied/EditorPch.hpp"
#include "VariableWindow.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/SimpleScript/Core/FlyClass.hpp"
#include "NodeScript/SimpleScript/Core/Utilities/ScriptUtilities.hpp"
#include "NodeScript/SimpleScript/Core/DataType/DataTypeManager.hpp"
#include "NodeScript/SimpleScript/Core/Utilities/ScriptProxy.hpp"
#include "NodeScript/SimpleScript/Core/ScriptModifier.hpp"
#include "NodeScript/SimpleScript/Core/Global/ScriptGlobal.hpp"

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
				
				Fly::CreateVariable(myParentWindow.GetNodeContext().classView);
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

	void VariableWindow::ModifyVariablePopup(const Fly::VariableView& aVariable)
	{
		char buffer[35]{};
		strcpy_s(buffer, aVariable.GetName().c_str());

		if (ImGui::InputText("##VariableName", buffer, IM_ARRAYSIZE(buffer)))
		{
			Fly::SetVariableName(aVariable.GetID(), buffer, myParentWindow.GetNodeContext().classView);
		}

		int currentSelectedIndex = 0;

		std::vector<Fly::DataTypeID> dataTypeIDs;
		std::stringstream ss;

		int i = 0;
		for (const auto& [dataTypeID, obj] : Fly::Global::GetDataTypeManager().GetDataTypes())
		{
			ss << obj.mName << '\0';
			dataTypeIDs.push_back(dataTypeID);
			if (aVariable.GetDataType().GetID() == dataTypeID)
			{
				currentSelectedIndex = i;
			}
			++i;
		}

		std::string names = ss.str();

		ImGui::Separator();

		if (ImGui::Combo("##ChangeDataType", &currentSelectedIndex, names.c_str()))
		{
			SetVariableDataType(aVariable.GetID(), dataTypeIDs.at(currentSelectedIndex), myParentWindow.GetNodeContext().classView, nullptr);
		}

		ImGui::Text("Default value:");
		ImGui::SameLine();
		EditVariableDefaultValue(aVariable.GetID(), myParentWindow.GetNodeContext().classView, nullptr);

		ImGui::Separator();

		if (ImGui::Button("Create Getter"))
		{
			CreateGetterNode(myParentWindow.GetNodeContext().classView, *myParentWindow.GetNodeContext().nodeGraph, aVariable.GetID());
			ImGui::CloseCurrentPopup();

		}

		ImGui::SameLine();

		if (ImGui::Button("Create Setter"))
		{
			CreateSetterNode(myParentWindow.GetNodeContext().classView, *myParentWindow.GetNodeContext().nodeGraph, aVariable.GetID());
		}

		if (ImGui::Button("Delete Variable"))
		{
			DestroyVariable(aVariable.GetID(), myParentWindow.GetNodeContext().classView, nullptr);
		}
	}
}