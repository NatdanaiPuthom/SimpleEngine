#include "Editor/Precomplied/EditorPch.hpp"
#include "VariableWindow.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/SimpleScript/Core/Script.h"
#include "NodeScript/SimpleScript/Core/Utilities/ScriptUtilities.h"
#include "NodeScript/SimpleScript/Core/DataType/DataTypeManager.h"
#include "NodeScript/SimpleScript/Core/Utilities/ScriptProxy.h"
#include "NodeScript/SimpleScript/Core/ScriptModifier.h"
#include "NodeScript/SimpleScript/Core/Global/ScriptGlobal.h"

namespace Editor
{

	VariableWindow::VariableWindow(NodeScriptingWindow& aParentWindow)
		: myParentWindow(aParentWindow)
	{
	}

	void VariableWindow::Update()
	{
		SCRIPT::Script& currentScript = *myParentWindow.GetNodeContext().script;

		if (ImGui::Begin("VariableWindow"))
		{
			if (ImGui::Button("Create Variable"))
			{
				
				SCRIPT::CreateVariable(*myParentWindow.GetNodeContext().script);
			}

			ImGui::Separator();

			std::vector<SCRIPT::VariableView> variables = SCRIPT::GetVariables(currentScript);
			for (const SCRIPT::VariableView& variable : variables)
			{
				const std::string treeNodeStrID = std::string("##ScriptVariable" + std::to_string(variable.GetID()));
				if (ImGui::TreeNode(treeNodeStrID.c_str(), variable.GetName().c_str()))
				{
					ImGui::SameLine(ImGui::GetWindowWidth() - 30.f);

					ImGui::BeginDisabled();
					ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(SCRIPT::Global::GetDataTypeManager().GetColor(variable.GetDataTypeID()))));
					ImGui::EndDisabled();

					ModifyVariablePopup(variable);

					ImGui::TreePop();
				}
				else
				{
					ImGui::SameLine(ImGui::GetWindowWidth() - 30.f);

					ImGui::BeginDisabled();
					ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(SCRIPT::Global::GetDataTypeManager().GetColor(variable.GetDataTypeID()))));
					ImGui::EndDisabled();
				}

				ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 1));
				ImGui::Separator();
				ImGui::PopStyleColor();
			}

		}

		ImGui::End();
	}

	void VariableWindow::ModifyVariablePopup(const SCRIPT::VariableView& aVariable)
	{
		char buffer[35]{};
		strcpy_s(buffer, aVariable.GetName().c_str());

		if (ImGui::InputText("##VariableName", buffer, IM_ARRAYSIZE(buffer)))
		{
			SetVariableName(aVariable.GetID(), buffer, *myParentWindow.GetNodeContext().script);
		}

		int currentSelectedIndex = 0;

		std::vector<SCRIPT::DataTypeID> dataTypeIDs;
		std::stringstream ss;

		int i = 0;
		for (const auto& [dataTypeID, obj] : SCRIPT::Global::GetDataTypeManager().GetDataTypes())
		{
			ss << obj.name << '\0';
			dataTypeIDs.push_back(dataTypeID);
			if (aVariable.GetDataTypeID() == dataTypeID)
			{
				currentSelectedIndex = i;
			}
			++i;
		}

		std::string names = ss.str();

		ImGui::Separator();

		if (ImGui::Combo("##ChangeDataType", &currentSelectedIndex, names.c_str()))
		{
			SetVariableDataType(aVariable.GetID(), dataTypeIDs.at(currentSelectedIndex), *myParentWindow.GetNodeContext().script, nullptr);
		}

		ImGui::Text("Default value:");
		ImGui::SameLine();
		EditVariableDefaultValue(aVariable.GetID(), *myParentWindow.GetNodeContext().script, nullptr);

		ImGui::Separator();

		if (ImGui::Button("Create Getter"))
		{
			CreateGetterNode(*myParentWindow.GetNodeContext().script, *myParentWindow.GetNodeContext().nodeGraph, aVariable.GetID());
			ImGui::CloseCurrentPopup();

		}

		ImGui::SameLine();

		if (ImGui::Button("Create Setter"))
		{
			CreateSetterNode(*myParentWindow.GetNodeContext().script, *myParentWindow.GetNodeContext().nodeGraph, aVariable.GetID());
		}

		if (ImGui::Button("Delete Variable"))
		{
			DestroyVariable(aVariable.GetID(), *myParentWindow.GetNodeContext().script, nullptr);
		}
	}
}