#include "NodeScript/Precomplied/NodeScriptPch.hpp"
#include "FunctionSettingsWindow.h"
#include "VisualScriptingWindow.h"
#include "NodeScript/SimpleScript/Core/Global/ScriptGlobal.h"
#include "NodeScript/SimpleScript/Core/DataType/DataTypeManager.h"
#include "NodeScript/SimpleScript/Core/Node/NodeTypeManager.h"
#include "NodeScript/SimpleScript/Core/Pin/PinTypeManager.h"
#include "NodeScript/SimpleScript/Core/Script.h"
#include "NodeScript/SimpleScript/Core/Function/ScriptFunction.h"

namespace Editor
{

	FunctionSettingsWindow::FunctionSettingsWindow(VisualScriptingWindow& aParent)
		: myParent(aParent)
	{
	}

	FunctionSettingsWindow::~FunctionSettingsWindow()
	{
	}

	void FunctionSettingsWindow::Update()
	{
		if (myParent.GetCurrentFunctionID() == SCRIPT::InvalidID<SCRIPT::FunctionID>())
		{
			return;
		}

		if (ImGui::Begin("Function Settings"))
		{
			ImGui::Text("Inputs");
			ImGui::SameLine();
			if (ImGui::Button("Add Input"))
			{
				SCRIPT::Modify::AddPinToFunction(myParent.GetCurrentFunctionID(), SCRIPT::GetDataTypeID<bool>(), SCRIPT::eFlowType::Input);
			}
			ImGui::Separator();

			ShowInputOutput(SCRIPT::eFlowType::Input);

			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Text("Outputs");
			ImGui::SameLine();
			if (ImGui::Button("Add Output"))
			{
				SCRIPT::Modify::AddPinToFunction(myParent.GetCurrentFunctionID(), SCRIPT::GetDataTypeID<bool>(), SCRIPT::eFlowType::Output);
			}
			ImGui::Separator();

			ShowInputOutput(SCRIPT::eFlowType::Output);


		}
		ImGui::End();
	}

	void FunctionSettingsWindow::ShowInputOutput(SCRIPT::eFlowType aFlowType)
	{
		SCRIPT::NodeTypeManager& nodeTypeManager = SCRIPT::Global::GetNodeTypeManager();
		SCRIPT::DataTypeManager& dataTypeManager = SCRIPT::Global::GetDataTypeManager();
		SCRIPT::PinTypeManager& pinTypeManager = SCRIPT::Global::GetPinTypeManager();
		const SCRIPT::Function& function = nodeTypeManager.GetFunction(myParent.GetCurrentFunctionID());

		const SCRIPT::NodeTypeID callerNodeTypeID = function.GetCallerNodeTypeID();
		const SCRIPT::NodeType& callerNodeType = nodeTypeManager.GetNodeType(callerNodeTypeID);

		const std::vector<SCRIPT::PinTypeID>& pinTypeIDs = SCRIPT::SelectByFlowType(aFlowType, callerNodeType.nodeRecipe.inputPinTypeIDs, callerNodeType.nodeRecipe.outputPinTypeIDs);
		for (size_t i = 0; i < pinTypeIDs.size(); ++i)
		{
			const SCRIPT::PinTypeID pinTypeID = pinTypeIDs[i];
			const SCRIPT::PinType& inputPinType = pinTypeManager.GetPinType(pinTypeID);
			const SCRIPT::DataType* pinTypeDataType = dataTypeManager.Find(inputPinType.dataTypeID);
			constexpr static const char* comboLabel1 = "Data Type##FunctionSettings_";
			const std::string inputOutputLabel = SCRIPT::SelectByFlowType(aFlowType, std::string("Input"), std::string("Output"));
			const std::string comboLabel = comboLabel1 + inputOutputLabel + std::to_string(i);
			if (ImGui::BeginCombo(comboLabel.c_str(), pinTypeDataType->name.c_str()))
			{
				size_t dataTypeIndex = 0;
				for (const auto& [dataTypeID, dataType] : dataTypeManager.GetDataTypes())
				{
					if (ImGui::Selectable(dataType.name.c_str()))
					{
						SCRIPT::Modify::SetPinAtIndexFunction(myParent.GetCurrentFunctionID(), i, dataTypeID, aFlowType);
					}
					++dataTypeIndex;
				}
				ImGui::EndCombo();
			}

		}
	}
}
