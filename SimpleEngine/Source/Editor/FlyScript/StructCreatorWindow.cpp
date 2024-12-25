#include "Editor/Precomplied/EditorPch.hpp"
#include "StructCreatorWindow.hpp"
#include "NodeScriptingWindow.hpp"
#include "FlyScriptEditorUtilities.hpp"

namespace Editor
{
	StructCreatorWindow::StructCreatorWindow(NodeScriptingWindow* aParentWindow)
		: myParentWindow(aParentWindow)
	{
	}
	void StructCreatorWindow::Update()
	{
		if (!myStructFacade)
		{
			return;
		}

		if (ImGui::Begin(std::string("Create Struct - " + std::string(myStructFacade.GetName())).c_str()))
		{


			if (ImGui::Button("Create Variable"))
			{
				myStructFacade.CreateVariable(Fly::DataTypeFacade(Fly::GetDataTypeID<bool>()), "Var", nullptr);
			}

			ImGui::Separator();

			auto members = myStructFacade.GetMemberVariables();


			for (auto& member : members)
			{
				ShowStructMember(member);

				ImGui::Separator();
			}
		}

		ImGui::End();
	}

	void StructCreatorWindow::ShowStructMember(Fly::VariableFacade aVariable)
	{

		Fly::VarID varID = aVariable.GetID();
		std::string name = aVariable.GetName();

		if (ImGui::InputString<32>(Combine("##StructVariableName", varID).c_str(), name))
		{
			aVariable.SetName(name, &myParentWindow->GetCommandTracker());
		}

		Fly::DataTypeFacade currentDataType = aVariable.GetDataType();
		if (DataTypeComboEditableFilter(Combine("##VarDataType", varID).c_str(), currentDataType))
		{
			aVariable.SetDataType(currentDataType, &myParentWindow->GetCommandTracker());
		}

		aVariable.ViewAndEditDefaultValue(&myParentWindow->GetCommandTracker());

		ImGui::SameLine();

		if (ImGui::Button(Combine("Destroy##DestroyVariable", varID).c_str()))
		{
			aVariable.Destroy(&myParentWindow->GetCommandTracker());

		}
	}
}
