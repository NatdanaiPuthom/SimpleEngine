#include "Editor/Precomplied/EditorPch.hpp"
#include "FlyScriptEditorUtilities.hpp"
#include "NodeScript/SimpleScript/Core/Fly.hpp"


namespace Editor
{

	static bool DataTypeCombo(const char* aComboLabel, Fly::DataTypeFacade& aDataTypeFacade, auto aFilterPredicate)
	{
		bool wasSelected = false;
		if (ImGui::BeginCombo(aComboLabel, aDataTypeFacade.GetName().c_str()))
		{

			const std::vector<Fly::DataTypeFacade> dataTypes = Fly::GetDataTypesFiltered(aFilterPredicate);

			for (const Fly::DataTypeFacade& dataType : dataTypes)
			{
				if (ImGui::Selectable(dataType.GetName().c_str()))
				{
					aDataTypeFacade = dataType;
					wasSelected = true;
				}
			}

			ImGui::EndCombo();
		}
		return wasSelected;
	}

	bool DataTypeComboEditableFilter(const char* aComboLabel, Fly::DataTypeFacade& aDataTypeFacade)
	{
		return DataTypeCombo(aComboLabel, aDataTypeFacade, [](const Fly::DataTypeFacade& aDataType) -> bool { return aDataType.IsEditable(); });
	}

	bool DataTypeComboTargetableFilter(const char* aComboLabel, Fly::DataTypeFacade& aDataTypeFacade)
	{
		return DataTypeCombo(aComboLabel, aDataTypeFacade, [](const Fly::DataTypeFacade& aDataType) -> bool { return aDataType.IsTargetable(); });
	}
}
