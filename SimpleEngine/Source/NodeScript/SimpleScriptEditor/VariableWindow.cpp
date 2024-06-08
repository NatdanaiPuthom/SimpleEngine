#include "SimpleScriptEditor/VariableWindow.h"
#include "SimpleScriptEditor/VisualScriptingWindow.h"
#include "SimpleScript/Core/Script.h"
#include "SimpleScript/Core/Utilities/ScriptUtilities.h"
#include "SimpleScript/Core/DataType/DataTypeManager.h"
#include "SimpleScript/Core/Utilities/ScriptProxy.h"

namespace Editor
{
	using namespace SCRIPT;

	VariableWindow::VariableWindow(VisualScriptingWindow& aParentWindow)
		: myParentWindow(aParentWindow)
	{
	}

	void VariableWindow::Update()
	{
		Script& currentScript = *myParentWindow.GetCurrentContext().script;
		ScriptModifier& modifier = currentScript.GetModifier();

		if (ImGui::Begin("VariableWindow"))
		{
			if (ImGui::Button("Create Variable"))
			{
				
				modifier.CreateVariable();
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
					ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(DataTypeManager::GetColor(variable.dataTypeID))));
					ImGui::EndDisabled();

					ModifyVariablePopup(i);

					ImGui::TreePop();
				}
				else
				{
					ImGui::SameLine(ImGui::GetWindowWidth() - 30.f);

					ImGui::BeginDisabled();
					ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(DataTypeManager::GetColor(variable.dataTypeID))));
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
		Script& currentScript = *myParentWindow.GetCurrentContext().script;
		const Variable& variable = ScriptProxy::GetVariable(currentScript, aVarID);
		std::string variableName = ScriptProxy::GetVariable(currentScript, aVarID).name;

		char buffer[35]{};
		strcpy_s(buffer, variableName.c_str());

		if (ImGui::InputText("##VariableName", buffer, IM_ARRAYSIZE(buffer)))
		{
			variableName = buffer;
			currentScript.GetModifier().SetVariableName(aVarID, variableName);
		}

		int currentSelectedIndex = 0;

		std::vector<DataTypeID> dataTypeIDs;
		std::stringstream ss;

		int i = 0;
		for (const auto& [dataTypeID, obj] : DataTypeManager::GetObjectTypes())
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
			currentScript.GetModifier().SetVariableDataType(aVarID, dataTypeIDs.at(currentSelectedIndex));
		}

		ImGui::Text("Default value:");
		ImGui::SameLine();
		currentScript.GetModifier().EditVariableDefaultValue(aVarID);

		ImGui::Separator();

		if (ImGui::Button("Create Getter"))
		{
			currentScript.GetModifier().CreateGetterNode(variable.dataTypeID, aVarID);
			ImGui::CloseCurrentPopup();

		}

		ImGui::SameLine();

		if (ImGui::Button("Create Setter"))
		{
			currentScript.GetModifier().CreateSetterNode(variable.dataTypeID, aVarID);
		}

		if (ImGui::Button("Delete Variable"))
		{
			ScriptModifier& modifier = currentScript.GetModifier();
			modifier.DestroyVariable(aVarID);
		}
	}
}