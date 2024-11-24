#include "Editor/Precomplied/EditorPch.hpp"
#include "CustomEventWindow.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/Fly.hpp"
#include "NodeScript/Global/FlyGlobal.hpp"
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
		if (ImGui::Begin("Custom Event"))
		{
			if (ImGui::Button("Create Custom Event"))
			{
				Fly::CreateCustomEvent("CustomEvent");
			}

			std::vector<Fly::CustomEventFacade> customEvents = Fly::GetCustomEvents();

			for (Fly::CustomEventFacade& customEvent : customEvents)
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

	void CustomEventWindow::EditInputs(Fly::CustomEventFacade& aCustomEventFacade)
	{
		const Fly::NodeTypeFacade executorNodeType = aCustomEventFacade.GetExecutorNodeType();
		std::string shortName = executorNodeType.GetShortName();

		char nameBuffer[35]{};
		strcpy_s(nameBuffer, shortName.c_str());

		if (ImGui::InputText(std::string("##CustomEventName" + std::to_string(aCustomEventFacade.GetID())).c_str(), nameBuffer, IM_ARRAYSIZE(nameBuffer)))
		{
			aCustomEventFacade.SetName(nameBuffer, nullptr);
		}

		ImGui::SameLine();

		if (ImGui::Button("Add Pin"))
		{
			aCustomEventFacade.AddPin(Fly::DataTypeFacade(Fly::GetDataTypeID<bool>()), "Pin", nullptr);
		}

		ImGui::Separator();
		
		const std::vector<Fly::PinTypeFacade> outputPinTypes = executorNodeType.GetOutputPinTypeFacades();
		for (size_t i = 0; i < outputPinTypes.size(); ++i)
		{
			if (i == 0)
			{
				continue;
			}
			const Fly::PinTypeFacade& outputPinType = outputPinTypes.at(i);

			ImGui::Text("%u:", i);
			ImGui::SameLine();

			char buffer[35]{};
			strcpy_s(buffer, outputPinType.GetName().c_str());

			if (ImGui::InputText(("##CustomEventPinName" + std::to_string(i)).c_str(), buffer, IM_ARRAYSIZE(buffer)))
			{
				aCustomEventFacade.SetPinNameAtIndex(buffer, i, nullptr);
			}

			ImGui::SameLine();
			if (ImGui::Button(std::string("Delete##" + std::to_string(i)).c_str()))
			{
				aCustomEventFacade.DeletePinAtIndex(i, nullptr);
			}

			const std::string comboLabel = "##CustomEventPinType" + std::to_string(i);
			Fly::DataTypeFacade currentDataTypeFacade(outputPinType.GetDataTypeID());
			if (DataTypeComboEditableFilter(comboLabel.c_str(), currentDataTypeFacade))
			{
				aCustomEventFacade.SetPinDataTypeAtIndex(currentDataTypeFacade, i, nullptr);
			}

			ImGui::SameLine();

			ImGui::BeginDisabled();
			ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(currentDataTypeFacade.GetColor())));
			ImGui::EndDisabled();
		}

		ImGui::Separator();

		if (ImGui::Button("Create Caller"))
		{
			myParentWindow.GetNodeContext().myNodeGraphFacade.CreateNode(aCustomEventFacade.GetCallerNodeType());
		}

		ImGui::SameLine();

		if (ImGui::Button("Create Executor"))
		{
			myParentWindow.GetNodeContext().myNodeGraphFacade.CreateNode(aCustomEventFacade.GetExecutorNodeType());
		}
	}
}