#include "Editor/Precomplied/EditorPch.hpp"
#include "FunctionSettingsWindow.hpp"
#include "NodeScriptingWindow.hpp"
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
		if (!myParent.GetCurrentFunctionView())
		{
			return;
		}

		if (ImGui::Begin("Function Settings"))
		{
			const std::string& functionName = myParent.GetCurrentFunctionView().GetName();
			char nameBuffer[40]{};
			strcpy_s(nameBuffer, functionName.c_str());
			if (ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer)))
			{
				myParent.GetCurrentFunctionView().SetName(nameBuffer, nullptr);
			}

			ImGui::Separator();

			ImGui::Text("Inputs");
			ImGui::SameLine();
			if (ImGui::Button("Add Input"))
			{
				myParent.GetCurrentFunctionView().AddPin(Fly::DataTypeView(Fly::GetDataTypeID<bool>()), Fly::eFlowType::Input, "Pin", nullptr);
			}
			ImGui::Separator();

			ShowInputOutput(Fly::eFlowType::Input);

			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Text("Outputs");
			ImGui::SameLine();
			if (ImGui::Button("Add Output"))
			{
				myParent.GetCurrentFunctionView().AddPin(Fly::DataTypeView(Fly::GetDataTypeID<bool>()), Fly::eFlowType::Output, "Pin", nullptr);
			}
			ImGui::Separator();

			ShowInputOutput(Fly::eFlowType::Output);


		}
		ImGui::End();
	}

	void FunctionSettingsWindow::ShowInputOutput(const Fly::eFlowType aFlowType)
	{
		const std::string inputOutputLabel = Fly::SelectByFlowType(aFlowType, std::string("Input"), std::string("Output"));

		constexpr static const char* dataTypeStrID = "Data Type##FunctionSettings_";
		constexpr static const char* pinTypeNameStrID = "Name##FunctionSettings_";

		Fly::FunctionView functionView = myParent.GetCurrentFunctionView();
		const Fly::NodeTypeView callerNodeTypeView = functionView.GetCallerNodeTypeView();

		const std::vector<Fly::PinTypeView> pinTypes = Fly::SelectByFlowType(aFlowType, callerNodeTypeView.GetInputPinTypes(), callerNodeTypeView.GetOutputPinTypes());
		for (size_t i = 1; i < pinTypes.size(); ++i)
		{
			const Fly::PinTypeView& pinType = pinTypes.at(i);

			const std::string& pinTypeName = pinType.GetName();

			char newName[32]{};
			strcpy_s(newName, pinTypeName.c_str());
			if (ImGui::InputText(std::string(pinTypeNameStrID + inputOutputLabel + std::to_string(i)).c_str(), newName, IM_ARRAYSIZE(newName)))
			{
				functionView.SetPinNameAtIndex(newName, i, aFlowType, nullptr);
			}

			const std::string comboLabel = dataTypeStrID + inputOutputLabel + std::to_string(i);

			const Fly::DataTypeView pinTypeDataType = Fly::DataTypeView(pinType.GetDataTypeID());

			if (ImGui::BeginCombo(comboLabel.c_str(), pinTypeDataType.GetName().c_str()))
			{
				for (const Fly::DataTypeView& dataTypeView : Fly::GetDataTypes())
				{
					if (ImGui::Selectable(dataTypeView.GetName().c_str()))
					{
						functionView.SetPinDataTypeAtIndex(dataTypeView, i, aFlowType, nullptr);
					}
				}
				ImGui::EndCombo();
			}

		}
	}
}
