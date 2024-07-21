#include "Editor/Precomplied/EditorPch.hpp"
#include "CustomEventWindow.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/SimpleScript/Core/FlyClass.hpp"
#include "NodeScript/SimpleScript/Core/DataType/DataTypeManager.hpp"
#include "NodeScript/SimpleScript/Core/ScriptModifier.hpp"
#include "NodeScript/SimpleScript/Core/Global/FlyGlobal.hpp"
#include "FlyScriptEditorUtilities.hpp"

namespace Editor
{
	CustomEventWindow::CustomEventWindow(NodeScriptingWindow& aParentWindow)
		: myParentWindow(aParentWindow)
	{
	}

	CustomEventWindow::~CustomEventWindow()
	{
	}

	void CustomEventWindow::Update()
	{
		if (ImGui::Begin("Node Creator"))
		{
			if (ImGui::Button("Create Custom Event Node"))
			{
				Fly::CreateCustomEvent("CustomEvent");
			}

			const std::vector<Fly::CustomEventView> customEvents = Fly::GetCustomEvents();

			for (const Fly::CustomEventView& customEvent : customEvents)
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

	void CustomEventWindow::EditInputs(const Fly::CustomEventView& aCustomEventView)
	{
		const Fly::NodeTypeView executorNodeType = aCustomEventView.GetExecutorNodeType();
		const Fly::NodeTypeView callerNodeType = aCustomEventView.GetCallerNodeType();
		std::string shortName = executorNodeType.GetShortName();

		char nameBuffer[35]{};
		strcpy_s(nameBuffer, shortName.c_str());

		if (ImGui::InputText(std::string("##" + std::to_string(aCustomEventView.GetID())).c_str(), nameBuffer, IM_ARRAYSIZE(nameBuffer)))
		{
			Fly::SetCustomEventName(aCustomEventView.GetID(), nameBuffer);
		}

		ImGui::SameLine();

		if (ImGui::Button("Add Pin"))
		{
			Fly::AddPinToCustomEvent(Fly::GetDataTypeID<bool>(), aCustomEventView.GetID(), "Pin");
		}

		ImGui::Separator();
		
		const std::vector<Fly::PinTypeView> outputPinTypes = executorNodeType.GetOutputPinTypes();
		for (size_t i = 0; i < outputPinTypes.size(); ++i)
		{
			const Fly::PinTypeView& outputPinType = outputPinTypes.at(i);

			ImGui::Text("%u:", i);
			ImGui::SameLine();

			char buffer[35]{};
			strcpy_s(buffer, outputPinType.GetName().c_str());

			if (ImGui::InputText(("##" + std::to_string(i)).c_str(), buffer, IM_ARRAYSIZE(buffer)))
			{
				Fly::SetPinTypeName(outputPinType.GetID(), buffer);
				const std::vector<Fly::PinTypeView> callerInputPinTypes = callerNodeType.GetInputPinTypes();
				Fly::SetPinTypeName(callerInputPinTypes.at(i).GetID(), buffer);
			}

			ImGui::SameLine();
			if (ImGui::Button(std::string("Delete##" + std::to_string(i)).c_str()))
			{
				Fly::DeletePinAtIndexCustomEvent(i, aCustomEventView.GetID());
			}

			const std::string comboLabel = "##CustomEventPinType" + std::to_string(i);
			Fly::DataTypeView currentDataType(outputPinType.GetDataTypeID());
			if (DataTypeComboEditableFilter(comboLabel.c_str(), currentDataType))
			{
				Fly::SetPinAtIndexCustomEvent(i, currentDataType, aCustomEventView.GetID());
			}

			ImGui::SameLine();

			ImGui::BeginDisabled();
			ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(currentDataType.GetColor())));
			ImGui::EndDisabled();
		}

		ImGui::Separator();

		if (ImGui::Button("Create Caller"))
		{
			CreateNode(*myParentWindow.GetNodeContext().nodeGraph, aCustomEventView.GetCallerNodeType());
		}

		ImGui::SameLine();

		if (ImGui::Button("Create Executor"))
		{
			CreateNode(*myParentWindow.GetNodeContext().nodeGraph, aCustomEventView.GetExecutorNodeType());
		}
	}
}