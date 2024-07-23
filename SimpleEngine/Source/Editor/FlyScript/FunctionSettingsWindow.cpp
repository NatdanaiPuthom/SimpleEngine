#include "Editor/Precomplied/EditorPch.hpp"
#include "FunctionSettingsWindow.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/SimpleScript/Core/Global/FlyGlobal.hpp"
#include "NodeScript/SimpleScript/Core/DataType/FlyDataTypeManager.hpp"
#include "NodeScript/SimpleScript/Core/Node/FlyNodeTypeManager.hpp"
#include "NodeScript/SimpleScript/Core/Pin/FlyPinTypeManager.hpp"
#include "NodeScript/SimpleScript/Core/FlyClass.hpp"
#include "NodeScript/SimpleScript/Core/Fly.hpp"

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
		if (!myParent.GetCurrentFunction())
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

		const Fly::FunctionView function(myParent.GetCurrentFunction());
		const Fly::NodeTypeView callerNodeType = function.GetCallerNodeType();

		const std::vector<Fly::PinTypeView> pinTypes = Fly::SelectByFlowType(aFlowType, callerNodeType.GetInputPinTypes(), callerNodeType.GetOutputPinTypes());
		for (size_t i = 0; i < pinTypes.size(); ++i)
		{
			const Fly::PinTypeView& pinType = pinTypes[i];
			const Fly::DataTypeView pinTypeDataType = Fly::DataTypeView(pinType.GetDataTypeID());
			constexpr static const char* comboLabel1 = "Data Type##FunctionSettings_";
			const std::string inputOutputLabel = Fly::SelectByFlowType(aFlowType, std::string("Input"), std::string("Output"));
			const std::string comboLabel = comboLabel1 + inputOutputLabel + std::to_string(i);
			if (ImGui::BeginCombo(comboLabel.c_str(), pinTypeDataType.GetName().c_str()))
			{
				for (const Fly::DataTypeView& dataTypeView : Fly::GetDataTypes())
				{
					if (ImGui::Selectable(dataTypeView.GetName().c_str()))
					{
						Fly::SetPinAtIndexFunction(myParent.GetCurrentFunction(), i, dataTypeView.GetID(), aFlowType);
					}
				}
				ImGui::EndCombo();
			}

		}
	}
}
