#include "Editor/Precomplied/EditorPch.hpp"
#include "FunctionSettingsWindow.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/Fly.hpp"

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
		if (!myParent.GetCurrentFunctionProxy())
		{
			return;
		}

		if (ImGui::Begin("Function Settings"))
		{
			const std::string& functionName = myParent.GetCurrentFunctionProxy().GetName();
			char nameBuffer[40]{};
			strcpy_s(nameBuffer, functionName.c_str());
			if (ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer)))
			{
				myParent.GetCurrentFunctionProxy().SetName(nameBuffer, nullptr);
			}

			ImGui::Separator();

			ImGui::Text("Inputs");
			ImGui::SameLine();
			if (ImGui::Button("Add Input"))
			{
				myParent.GetCurrentFunctionProxy().AddPin(Fly::GenericDataTypeProxy(Fly::GetDataTypeID<bool>()), Fly::eIODirection::Input, "Pin", nullptr);
			}
			ImGui::Separator();

			ShowInputOutput(Fly::eIODirection::Input);

			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Text("Outputs");
			ImGui::SameLine();
			if (ImGui::Button("Add Output"))
			{
				myParent.GetCurrentFunctionProxy().AddPin(Fly::GenericDataTypeProxy(Fly::GetDataTypeID<bool>()), Fly::eIODirection::Output, "Pin", nullptr);
			}
			ImGui::Separator();

			ShowInputOutput(Fly::eIODirection::Output);


		}
		ImGui::End();
	}

	void FunctionSettingsWindow::ShowInputOutput(const Fly::eIODirection aIODirection)
	{
		const std::string inputOutputLabel = Fly::SelectByIODirection(aIODirection, std::string("Input"), std::string("Output"));

		constexpr static const char* dataTypeStrID = "Data Type##FunctionSettings_";
		constexpr static const char* pinTypeNameStrID = "Name##FunctionSettings_";

		Fly::FunctionProxy functionProxy = myParent.GetCurrentFunctionProxy();
		const Fly::NodeTypeProxy callerNodeTypeProxy = functionProxy.GetCallerNodeType();

		const std::vector<Fly::PinTypeProxy> pinTypes = Fly::SelectByIODirection(aIODirection, callerNodeTypeProxy.GetInputPinTypes(), callerNodeTypeProxy.GetOutputPinTypes());
		for (size_t i = 1; i < pinTypes.size(); ++i)
		{
			const Fly::PinTypeProxy& pinType = pinTypes.at(i);

			const std::string& pinTypeName = pinType.GetName();

			char newName[32]{};
			strcpy_s(newName, pinTypeName.c_str());
			if (ImGui::InputText(std::string(pinTypeNameStrID + inputOutputLabel + std::to_string(i)).c_str(), newName, IM_ARRAYSIZE(newName)))
			{
				functionProxy.SetPinNameAtIndex(newName, i, aIODirection, nullptr);
			}

			const std::string comboLabel = dataTypeStrID + inputOutputLabel + std::to_string(i);

			const Fly::GenericDataTypeProxy pinTypeDataType(pinType.GetDataTypeID());

			if (ImGui::BeginCombo(comboLabel.c_str(), std::string(pinTypeDataType.GetName()).c_str()))
			{
				for (const Fly::GenericDataTypeProxy& dataTypeProxy : Fly::GetGenericDataTypes())
				{
					if (ImGui::Selectable(std::string(dataTypeProxy.GetName()).c_str()))
					{
						functionProxy.SetPinDataTypeAtIndex(dataTypeProxy, i, aIODirection, nullptr);
					}
				}
				ImGui::EndCombo();
			}

		}
	}
}
