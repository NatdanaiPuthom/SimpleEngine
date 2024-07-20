#include "Editor/Precomplied/EditorPch.hpp"
#include "FunctionSettingsWindow.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/SimpleScript/Core/Global/ScriptGlobal.hpp"
#include "NodeScript/SimpleScript/Core/DataType/DataTypeManager.hpp"
#include "NodeScript/SimpleScript/Core/Node/NodeTypeManager.hpp"
#include "NodeScript/SimpleScript/Core/Pin/PinTypeManager.hpp"
#include "NodeScript/SimpleScript/Core/FlyClass.hpp"

namespace Editor
{

	FunctionSettingsWindow::FunctionSettingsWindow(NodeScriptingWindow& aParent)
		: myParent(aParent)
	{
	}

	FunctionSettingsWindow::~FunctionSettingsWindow()
	{
	}

	void FunctionSettingsWindow::Update()
	{
		if (myParent.GetCurrentFunction() == Fly::InvalidID<Fly::FunctionID>())
		{
			return;
		}

		if (ImGui::Begin("Function Settings"))
		{
			ImGui::Text("Inputs");
			ImGui::SameLine();
			if (ImGui::Button("Add Input"))
			{
				Fly::AddPinToFunction(myParent.GetCurrentFunction(), Fly::GetDataTypeID<bool>(), Fly::eFlowType::Input);
			}
			ImGui::Separator();

			ShowInputOutput(Fly::eFlowType::Input);

			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Text("Outputs");
			ImGui::SameLine();
			if (ImGui::Button("Add Output"))
			{
				Fly::AddPinToFunction(myParent.GetCurrentFunction(), Fly::GetDataTypeID<bool>(), Fly::eFlowType::Output);
			}
			ImGui::Separator();

			ShowInputOutput(Fly::eFlowType::Output);


		}
		ImGui::End();
	}

	void FunctionSettingsWindow::ShowInputOutput(const Fly::eFlowType aFlowType)
	{
		Fly::DataTypeManager& dataTypeManager = Fly::Global::GetDataTypeManager();

		const Fly::FunctionView function(myParent.GetCurrentFunction());
		const Fly::NodeTypeView callerNodeType = function.GetCallerNodeType();

		const std::vector<Fly::PinTypeView> pinTypes = Fly::SelectByFlowType(aFlowType, callerNodeType.GetInputPinTypes(), callerNodeType.GetOutputPinTypes());
		for (size_t i = 0; i < pinTypes.size(); ++i)
		{
			const Fly::PinTypeView& pinType = pinTypes[i];
			const Fly::DataType* pinTypeDataType = dataTypeManager.Find(pinType.GetDataTypeID());
			constexpr static const char* comboLabel1 = "Data Type##FunctionSettings_";
			const std::string inputOutputLabel = Fly::SelectByFlowType(aFlowType, std::string("Input"), std::string("Output"));
			const std::string comboLabel = comboLabel1 + inputOutputLabel + std::to_string(i);
			if (ImGui::BeginCombo(comboLabel.c_str(), pinTypeDataType->mName.c_str()))
			{
				size_t dataTypeIndex = 0;
				for (const auto& [dataTypeID, dataType] : dataTypeManager.GetDataTypes())
				{
					if (ImGui::Selectable(dataType.mName.c_str()))
					{
						Fly::SetPinAtIndexFunction(myParent.GetCurrentFunction(), i, dataTypeID, aFlowType);
					}
					++dataTypeIndex;
				}
				ImGui::EndCombo();
			}

		}
	}
}
