#include "MathNodes.hpp"
#include "../FlyRegistration.hpp"
#include "../Internal/FlyFoundation.hpp"
#include <imgui.h>

namespace FLY_NAMESPACE
{

	ViewAndEditResult ViewAndEdit(StructInstance& aStructInstance)
	{
		ViewAndEditResult viewAndEditResult;

		std::string currentName = aStructInstance.GetStruct() ? aStructInstance.GetStruct()->mName : "None";

		if (ImGui::BeginCombo("##StructType", currentName.c_str()))
		{
			viewAndEditResult.mIsItemActive = true;
			const auto& structs = Internal::GetDataTypeManager().GetStructs();

			for (const auto& s : structs)
			{
				if (ImGui::Selectable(s->mName.c_str(), s.Get() == aStructInstance.GetStruct()))
				{
					aStructInstance = StructInstance(*s.Get());
				}
			}

			ImGui::EndCombo();
		}

		if (!aStructInstance)
		{
			return viewAndEditResult;
		}

		for (size_t i = 0; i < aStructInstance.mVariableContainerInstance.mVariableInstances.size(); ++i)
		{
			VariableInstance& variableInstance = aStructInstance.mVariableContainerInstance.mVariableInstances[i];

			const Variable& variable = aStructInstance.GetStruct()->mVariableContainer.mVariables[i];

			viewAndEditResult.mIsItemActive |= Internal::GetDataTypeManager().ViewAndEditData(variable.mDataTypeID, variableInstance.mDefaultValueDataPtr).mIsItemActive;


		}
		return viewAndEditResult;
	}


	StructInstance StructInstanceTest(const StructInstance& aStructInstance)
	{


		return aStructInstance;
	}


	FLY_VALUETYPE(StructInstance);
	FLY_FUNCTION(StructInstanceTest);
}

