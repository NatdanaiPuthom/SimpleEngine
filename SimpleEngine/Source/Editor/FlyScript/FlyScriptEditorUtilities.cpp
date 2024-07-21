#include "Editor/Precomplied/EditorPch.hpp"
#include "FlyScriptEditorUtilities.hpp"
#include "NodeScript/SimpleScript/Core/ScriptModifier.hpp"


namespace Editor
{

	static bool DataTypeCombo(const char* aComboLabel, Fly::DataTypeView& aDataTypeView, auto aFilterPredicate)
	{
		bool wasSelected = false;
		if (ImGui::BeginCombo(aComboLabel, aDataTypeView.GetName().c_str()))
		{

			const std::vector<Fly::DataTypeView> dataTypes = Fly::GetDataTypesFiltered(aFilterPredicate);

			for (const Fly::DataTypeView& dataType : dataTypes)
			{
				if (ImGui::Selectable(dataType.GetName().c_str()))
				{
					aDataTypeView = dataType;
					wasSelected = true;
				}
			}

			ImGui::EndCombo();
		}
		return wasSelected;
	}

	bool DataTypeComboEditableFilter(const char* aComboLabel, Fly::DataTypeView& aDataTypeView)
	{
		return DataTypeCombo(aComboLabel, aDataTypeView, [](const Fly::DataTypeView& aDataType) -> bool { return aDataType.IsEditable(); });
	}

	bool DataTypeComboTargetableFilter(const char* aComboLabel, Fly::DataTypeView& aDataTypeView)
	{
		return DataTypeCombo(aComboLabel, aDataTypeView, [](const Fly::DataTypeView& aDataType) -> bool { return aDataType.IsTargetable(); });
	}
}
