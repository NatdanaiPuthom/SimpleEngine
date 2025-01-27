#include "Editor/Precomplied/EditorPch.hpp"
#include "CustomEventWindow.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/Fly.hpp"
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

			std::vector<Fly::CustomEventProxy> customEvents = Fly::GetCustomEvents();

			for (Fly::CustomEventProxy& customEvent : customEvents)
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

	void CustomEventWindow::EditInputs(Fly::CustomEventProxy& aCustomEventProxy)
	{
		const Fly::NodeTypeProxy executorNodeType = aCustomEventProxy.GetExecutorNodeType();
		std::string shortName = executorNodeType.GetShortName();

		char nameBuffer[35]{};
		strcpy_s(nameBuffer, shortName.c_str());

		if (ImGui::InputText(std::string("##CustomEventName" + std::to_string(aCustomEventProxy.GetID())).c_str(), nameBuffer, IM_ARRAYSIZE(nameBuffer)))
		{
			aCustomEventProxy.SetName(nameBuffer, nullptr);
		}

		ImGui::SameLine();

		if (ImGui::Button("Add Pin"))
		{
			aCustomEventProxy.AddPin(Fly::GenericDataTypeProxy(Fly::GetDataTypeID<bool>()), "Pin", nullptr);
		}

		ImGui::Separator();
		
		const std::vector<Fly::PinTypeProxy> outputPinTypes = executorNodeType.GetOutputPinTypes();
		for (size_t i = 0; i < outputPinTypes.size(); ++i)
		{
			if (i == 0)
			{
				continue;
			}
			const Fly::PinTypeProxy& outputPinType = outputPinTypes.at(i);

			ImGui::Text("%u:", i);
			ImGui::SameLine();

			char buffer[35]{};
			strcpy_s(buffer, outputPinType.GetName().c_str());

			if (ImGui::InputText(("##CustomEventPinName" + std::to_string(i)).c_str(), buffer, IM_ARRAYSIZE(buffer)))
			{
				aCustomEventProxy.SetPinNameAtIndex(buffer, i, nullptr);
			}

			ImGui::SameLine();
			if (ImGui::Button(std::string("Delete##" + std::to_string(i)).c_str()))
			{
				aCustomEventProxy.DeletePinAtIndex(i, nullptr);
			}

			const std::string comboLabel = "##CustomEventPinType" + std::to_string(i);
			Fly::GenericDataTypeProxy currentDataTypeProxy(outputPinType.GetDataTypeID());
			if (DataTypeComboEditableFilter(comboLabel.c_str(), currentDataTypeProxy))
			{
				aCustomEventProxy.SetPinDataTypeAtIndex(currentDataTypeProxy, i, nullptr);
			}

			ImGui::SameLine();

			ImGui::BeginDisabled();
			ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(currentDataTypeProxy.GetColor())));
			ImGui::EndDisabled();
		}

		ImGui::Separator();

		if (ImGui::Button("Create Caller"))
		{
			myParentWindow.GetNodeContext().myNodeGraphProxy.CreateNode(aCustomEventProxy.GetCallerNodeType());
		}

		ImGui::SameLine();

		if (ImGui::Button("Create Executor"))
		{
			myParentWindow.GetNodeContext().myNodeGraphProxy.CreateNode(aCustomEventProxy.GetExecutorNodeType());
		}
	}
}