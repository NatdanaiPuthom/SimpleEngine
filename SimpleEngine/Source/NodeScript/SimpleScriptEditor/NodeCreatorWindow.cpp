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

			for (CustomEventID id = 0; id < NodeTypeManager::GetInstance().GetCustomEvents().size(); ++id)
			{
				const CustomEvent& customEvent = NodeTypeManager::GetInstance().GetCustomEvent(id);
				if (ImGui::TreeNode(std::to_string(id).c_str(), NodeTypeManager::GetInstance().GetShortName(customEvent.GetExecutorTypeID()).c_str()))
				{
					EditInputs(id);
					ImGui::TreePop();

				}

				ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.f, 0.f, 0.f, 1.f));
				ImGui::Separator();
				ImGui::PopStyleColor();
			}

		}

		ImGui::End();
	}

	void NodeCreatorWindow::EditInputs(SCRIPT::CustomEventID anID)
	{
		const CustomEvent& customEvent = NodeTypeManager::GetInstance().GetCustomEvent(anID);
		NodeTypeID executorTypeID = customEvent.GetExecutorTypeID();
		NodeTypeID callerTypeID = customEvent.GetCallerTypeID();
		NodeType& executorNodeType = NodeTypeManager::GetInstance().GetNodeType(executorTypeID);
		NodeType& callerNodeType = NodeTypeManager::GetInstance().GetNodeType(callerTypeID);

		std::string shortName = NodeTypeManager::GetInstance().GetShortName(executorTypeID);
		std::string nameDirectory = NodeTypeManager::GetInstance().GetNameDirectory(executorTypeID);

		char nodeTypeNameBuffer[35]{};
		strcpy_s(nodeTypeNameBuffer, shortName.c_str());

		if (ImGui::InputText(std::string("##" + std::to_string(anID)).c_str(), nodeTypeNameBuffer, IM_ARRAYSIZE(nodeTypeNameBuffer)))
		{
			executorNodeType.name = nameDirectory + nodeTypeNameBuffer;
			callerNodeType.name = nameDirectory + "Call " + nodeTypeNameBuffer;
		}

		ImGui::SameLine();

		if (ImGui::Button("Add Pin"))
		{
			Modify::AddPinToCustomEvent(typeid(bool).hash_code(), anID, "Pin");
		}

		ImGui::Separator();

		for (size_t i = 1; i < executorNodeType.nodeRecipe.outputPinTypeIDs.size(); ++i)
		{
			PinType& pinType = PinTypeManager::GetPinType(executorNodeType.nodeRecipe.outputPinTypeIDs[i]);
			int currentSelectedIndex = 0;

			std::vector<DataTypeID> dataTypeIDs;
			std::stringstream ss;

			int it = 0;
			for (const auto& [dataTypeID, dataType] : Global::GetDataTypeManager().GetFunctionDataTypes())
			{
				ss << dataType->name << '\0';
				dataTypeIDs.push_back(dataTypeID);
				if (PinTypeManager::GetPinType(executorNodeType.nodeRecipe.outputPinTypeIDs[i]).dataTypeID == dataTypeID)
				{
					currentSelectedIndex = it;
				}
				++it;
			}

			std::string names = ss.str();

			ImGui::Text("%u:", i);
			ImGui::SameLine();

			char buffer[35]{};
			strcpy_s(buffer, pinType.name.c_str());

			if (ImGui::InputText(("##" + std::to_string(i)).c_str(), buffer, IM_ARRAYSIZE(buffer)))
			{
				pinType.name = buffer;
				PinType& callerPinType = PinTypeManager::GetPinType(callerNodeType.nodeRecipe.inputPinTypeIDs[i]);
				callerPinType.name = pinType.name;
			}

			ImGui::SameLine();
			if (ImGui::Button(std::string("Delete##" + std::to_string(i)).c_str()))
			{
				Modify::DeletePinAtIndexCustomEvent(i, anID);
			}

			if (ImGui::Combo(std::string("##CustomEventPinType" + std::to_string(i)).c_str(), &currentSelectedIndex, names.c_str()))
			{

				Modify::SetPinAtIndexCustomEvent(i, dataTypeIDs.at(currentSelectedIndex), anID);
			}

			ImGui::SameLine();

			ImGui::BeginDisabled();
			ImGui::ColorButton("  ##Color", ImGui::ColorConvertU32ToFloat4(ToImGuiColor(Global::GetDataTypeManager().GetColor(dataTypeIDs.at(currentSelectedIndex)))));
			ImGui::EndDisabled();
		}

		ImGui::Separator();

		if (ImGui::Button("Create Caller"))
		{
			Modify::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, callerTypeID);
		}

		ImGui::SameLine();

		if (ImGui::Button("Create Executor"))
		{
			Modify::CreateNode(*myParentWindow.GetNodeContext().nodeGraph, executorTypeID);
		}
	}
}