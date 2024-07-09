#include "SimpleScriptEditor/VariableWindow.h"
#include "SimpleScriptEditor/VisualScriptingWindow.h"
#include "SimpleScript/Core/Script.h"
#include "SimpleScript/Core/Utilities/ScriptUtilities.h"
#include "SimpleScript/Core/DataType/DataTypeManager.h"
#include "SimpleScript/Core/Utilities/ScriptProxy.h"
#include "SimpleScript/Core/ScriptModifier.h"
#include "SimpleScript/Core/Global/ScriptGlobal.h"

namespace Editor
{
	using namespace SCRIPT;

	VariableWindow::VariableWindow(VisualScriptingWindow& aParentWindow)
		: myParentWindow(aParentWindow)
	{
	}

	void VariableWindow::Update()
	{
		Script& currentScript = *myParentWindow.GetNodeContext().script;

		if (ImGui::Begin("VariableWindow"))
		{
			if (ImGui::Button("Create Variable"))
			{
				
				Modify::CreateVariable(*myParentWindow.GetNodeContext().script);
			}

			ImGui::Separator();

			for (VarID i = 0; i < ScriptProxy::GetVariables(currentScript).size(); ++i)
			{
				const Variable& variable = ScriptProxy::GetVariable(currentScript, i);

				if (variable.isDestroyed)
					continue;

				if (ImGui::TreeNode(std::to_string(i).c_str(), variable.name.c_str()))
				{
					ImGui::SameLine(ImGui::GetWindowWidth() - 30.f);

					ImGui::BeginDisabled();
					ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(Global::GetDataTypeManager().GetColor(variable.dataTypeID))));
					ImGui::EndDisabled();

					ModifyVariablePopup(i);

					ImGui::TreePop();
				}
				else
				{
					ImGui::SameLine(ImGui::GetWindowWidth() - 30.f);

					ImGui::BeginDisabled();
					ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(Global::GetDataTypeManager().GetColor(variable.dataTypeID))));
					ImGui::EndDisabled();
				}

				ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 1));
				ImGui::Separator();
				ImGui::PopStyleColor();
			}

		}

		ImGui::End();
	}

	void VariableWindow::ModifyVariablePopup(VarID aVarID)
	{
		Script& currentScript = *myParentWindow.GetNodeContext().script;
		const Variable& variable = ScriptProxy::GetVariable(currentScript, aVarID);
		std::string variableName = ScriptProxy::GetVariable(currentScript, aVarID).name;

		char buffer[35]{};
		strcpy_s(buffer, variableName.c_str());

		if (ImGui::InputText("##VariableName", buffer, IM_ARRAYSIZE(buffer)))
		{
			variableName = buffer;
			Modify::SetVariableName(aVarID, variableName, *myParentWindow.GetNodeContext().script);
		}

		int currentSelectedIndex = 0;

		std::vector<DataTypeID> dataTypeIDs;
		std::stringstream ss;

		int i = 0;
		for (const auto& [dataTypeID, obj] : Global::GetDataTypeManager().GetDataTypes())
		{
			ss << obj.name << '\0';
			dataTypeIDs.push_back(dataTypeID);
			if (variable.dataTypeID == dataTypeID)
			{
				currentSelectedIndex = i;
			}
			++i;
		}

		std::string names = ss.str();

		ImGui::Separator();

		if (ImGui::Combo("##ChangeDataType", &currentSelectedIndex, names.c_str()))
		{
			Modify::SetVariableDataType(aVarID, dataTypeIDs.at(currentSelectedIndex), *myParentWindow.GetNodeContext().script, nullptr);
		}

		ImGui::Text("Default value:");
		ImGui::SameLine();
		Modify::EditVariableDefaultValue(aVarID, *myParentWindow.GetNodeContext().script, nullptr);

		ImGui::Separator();

		if (ImGui::Button("Create Getter"))
		{
			Modify::CreateGetterNode(*myParentWindow.GetNodeContext().script, *myParentWindow.GetNodeContext().nodeGraph, variable.dataTypeID, aVarID);
			ImGui::CloseCurrentPopup();

		}

		ImGui::SameLine();

		if (ImGui::Button("Create Setter"))
		{
			Modify::CreateGetterNode(*myParentWindow.GetNodeContext().script, *myParentWindow.GetNodeContext().nodeGraph, variable.dataTypeID, aVarID);
		}

		if (ImGui::Button("Delete Variable"))
		{
			Modify::DestroyVariable(aVarID, *myParentWindow.GetNodeContext().script, nullptr);
		}
	}
}