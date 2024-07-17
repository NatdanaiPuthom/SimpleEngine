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

	void FunctionSettingsWindow::ShowInputOutput(const SCRIPT::eFlowType aFlowType)
	{
		SCRIPT::DataTypeManager& dataTypeManager = SCRIPT::Global::GetDataTypeManager();

		const SCRIPT::FunctionView function(myParent.GetCurrentFunctionID());
		const SCRIPT::NodeTypeView callerNodeType = function.GetCallerNodeType();

		const std::vector<SCRIPT::PinTypeView> pinTypes = SCRIPT::SelectByFlowType(aFlowType, callerNodeType.GetInputPinTypes(), callerNodeType.GetOutputPinTypes());
		for (size_t i = 0; i < pinTypes.size(); ++i)
		{
			const SCRIPT::PinTypeView& pinType = pinTypes[i];
			const SCRIPT::DataType* pinTypeDataType = dataTypeManager.Find(pinType.GetDataTypeID());
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
