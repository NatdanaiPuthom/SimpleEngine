#include "SimpleScriptEditor/NodeCreatorWindow.h"
#include "SimpleScriptEditor/VisualScriptingWindow.h"
#include "SimpleScript/Core/Script.h"
#include "SimpleScript/Core/Node/NodeTypeManager.h"
#include "SimpleScript/Core/Pin/PinTypeManager.h"
#include "SimpleScript/Core/DataType/DataTypeManager.h"
#include "SimpleScript/Core/Utilities/ScriptProxy.h"
#include "SimpleScript/Core/ScriptModifier.h"
#include "SimpleScript/Core/Global/ScriptGlobal.h"

using namespace SCRIPT;

namespace Editor
{
	NodeCreatorWindow::NodeCreatorWindow(VisualScriptingWindow& aParentWindow)
		: myParentWindow(aParentWindow)
	{
	}

	NodeCreatorWindow::~NodeCreatorWindow()
	{
	}

	void NodeCreatorWindow::Update()
	{
		if (ImGui::Begin("Node Creator"))
		{
			if (ImGui::Button("Create Custom Event Node"))
			{
				Modify::CreateCustomEvent("CustomEvent");
			}

			const std::vector<CustomEventView> customEvents = Modify::GetCustomEvents();

			for (const CustomEventView& customEvent : customEvents)
			{
				if (ImGui::TreeNode(std::to_string(customEvent.GetID()).c_str(), customEvent.GetExecutorNodeType().GetShortName().c_str()))
				{
					EditInputs(customEvent);
					ImGui::TreePop();

				}

				ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.f, 0.f, 0.f, 1.f));
				ImGui::Separator();
				ImGui::PopStyleColor();
			}

		}

		ImGui::End();
	}

	void NodeCreatorWindow::EditInputs(const SCRIPT::CustomEventView& aCustomEventView)
	{
		const NodeTypeView executorNodeType = aCustomEventView.GetExecutorNodeType();
		const NodeTypeView callerNodeType = aCustomEventView.GetCallerNodeType();
		std::string shortName = executorNodeType.GetShortName();

		char nameBuffer[35]{};
		strcpy_s(nameBuffer, shortName.c_str());

		if (ImGui::InputText(std::string("##" + std::to_string(aCustomEventView.GetID())).c_str(), nameBuffer, IM_ARRAYSIZE(nameBuffer)))
		{
			SCRIPT::Modify::SetCustomEventName(aCustomEventView.GetID(), nameBuffer);
		}

		ImGui::SameLine();

		if (ImGui::Button("Add Pin"))
		{
			Modify::AddPinToCustomEvent(GetDataTypeID<bool>(), aCustomEventView.GetID(), "Pin");
		}

		ImGui::Separator();
		
		const std::vector<PinTypeView> outputPinTypes = executorNodeType.GetOutputPinTypes();
		for (size_t i = 1; i < outputPinTypes.size(); ++i)
		{
			const PinTypeView& pinType = outputPinTypes.at(i);
			int currentSelectedIndex = 0;

			std::vector<DataTypeID> dataTypeIDs;
			std::stringstream ss;

			int it = 0;
			for (const auto& [dataTypeID, dataType] : Global::GetDataTypeManager().GetFunctionDataTypes())
			{
				ss << dataType->name << '\0';
				dataTypeIDs.push_back(dataTypeID);
				if (pinType.GetDataTypeID()  == dataTypeID)
				{
					currentSelectedIndex = it;
				}
				++it;
			}

			std::string names = ss.str();

			ImGui::Text("%u:", i);
			ImGui::SameLine();

			char buffer[35]{};
			strcpy_s(buffer, pinType.GetName().c_str());

			if (ImGui::InputText(("##" + std::to_string(i)).c_str(), buffer, IM_ARRAYSIZE(buffer)))
			{
				Modify::SetPinTypeName(pinType.GetID(), buffer);
				const std::vector<PinTypeView> callerInputPinTypes = callerNodeType.GetInputPinTypes();
				Modify::SetPinTypeName(callerInputPinTypes.at(i).GetID(), buffer);
			}

			ImGui::SameLine();
			if (ImGui::Button(std::string("Delete##" + std::to_string(i)).c_str()))
			{
				Modify::DeletePinAtIndexCustomEvent(i, aCustomEventView.GetID());
			}

			if (ImGui::Combo(std::string("##CustomEventPinType" + std::to_string(i)).c_str(), &currentSelectedIndex, names.c_str()))
			{

				Modify::SetPinAtIndexCustomEvent(i, dataTypeIDs.at(currentSelectedIndex), aCustomEventView.GetID());
			}

			ImGui::SameLine();

			ImGui::BeginDisabled();
			ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(Global::GetDataTypeManager().GetColor(dataTypeIDs.at(currentSelectedIndex)))));
			ImGui::EndDisabled();
		}

		ImGui::Separator();

		if (ImGui::Button("Create Caller"))
		{
			Modify::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, aCustomEventView.GetCallerNodeType().GetID());
		}

		ImGui::SameLine();

		if (ImGui::Button("Create Executor"))
		{
			Modify::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, aCustomEventView.GetExecutorNodeType().GetID());
		}
	}
}