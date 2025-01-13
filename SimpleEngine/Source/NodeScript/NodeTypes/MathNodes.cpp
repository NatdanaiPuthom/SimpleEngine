#include "MathNodes.hpp"
#include "../FlyRegistration.hpp"
#include "../Internal/FlyFoundation.hpp"
#include <imgui.h>

namespace FLY_NAMESPACE
{


	void TestUtilityFunction()
	{

	}

	ViewAndEditResult ViewAndEdit(StructInstance& aStructInstance)
	{
		ViewAndEditResult viewAndEditResult;
		aStructInstance;

		/*std::string currentName = aStructInstance.GetStruct() ? aStructInstance.GetStruct()->mName : "None";

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


		}*/
		return viewAndEditResult;
	}


	StructInstance StructInstanceTest(const StructInstance& aStructInstance)
	{


		return aStructInstance;
	}


	FLY_VALUETYPE(StructInstance);
	FLY_FUNCTION(StructInstanceTest);

	/*ViewAndEditResult ViewAndEdit(TraitObject& aValue)
	{
		aValue;

		ImGui::Text("%s", Internal::GetTraitByID(aValue.mTraitID).mName.c_str());
		ImGui::Text("%s", Internal::GetNodeType(Internal::GetTraitByID(aValue.mTraitID).mFunctions[aValue.mFunctionIndex].mCallerNodeTypeID).mNodeRecipe.mName.c_str());

		return ViewAndEditResult{};

	}*/

	FLY_VALUETYPE(TraitObject);


	struct TestTrait
	{
		float Test(int)
		{
			return 0.0f;
		}
	};


	float BoolTest(bool*, int)
	{
		return 3.f;
	}

	float IntTest(int*, int)
	{
		return 2;
	}


	FLY_TRAIT(TestTrait);
	FLY_TRAIT_FUNCTION(TestTrait::Test);
	FLY_TRAIT_IMPLEMENTATION(BoolTest, TraitImplOf<TestTrait>);
	FLY_TRAIT_IMPLEMENTATION(IntTest, TraitImplOf<TestTrait>);


	struct Tickable final
	{
		Flow Tick()
		{
			return Flow(true);
		}
	};


	FLY_TRAIT(Tickable);
	FLY_TRAIT_FUNCTION(Tickable::Tick, Pure{});
}

