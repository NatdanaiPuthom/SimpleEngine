#include "MathNodes.hpp"
#include "../FlyRegistration.hpp"
#include "../Internal/FlyFoundation.hpp"
#include <imgui.h>

namespace std
{
	template<typename T, typename... Rest>
	Fly::ViewAndEditResult ViewAndEditVariantType(auto& aVariant)
	{
		const Fly::DataTypeID dataTypeID = Fly::GetDataTypeID<T>();
		const Fly::DataType* dataType = Fly::Internal::GetDataTypeByID(dataTypeID);

		if (dataType)
		{

			if (T* value = std::get_if<T>(&aVariant))
			{
				return Fly::Internal::GetDataTypeManager().ViewAndEditData(dataTypeID, value);
			}
		}

		if constexpr (sizeof...(Rest) > 0)
		{
			return ViewAndEditVariantType<Rest...>(aVariant);
		}

		return Fly::ViewAndEditResult{};
	}

	template<typename T, typename... Rest>
	void ShowSelectableVariantTypes(auto& aVariant, const size_t aCurrentIndex = 0)
	{
		const std::string& name = Fly::Internal::GetDataTypeManager().GetName(Fly::GetDataTypeID<T>());

		const bool isSelected = aCurrentIndex == aVariant.index();
		if (ImGui::Selectable(name.c_str(), isSelected))
		{
			if (!isSelected)
			{
				aVariant = T{};
			}
		}

		if constexpr (sizeof...(Rest) > 0)
		{
			ShowSelectableVariantTypes<Rest...>(aVariant, aCurrentIndex + 1);
		}
	}

	template<typename... Types>
	Fly::ViewAndEditResult ViewAndEdit(std::variant<Types...>& aVariant)
	{
		Fly::ViewAndEditResult viewAndEditResult;
		const Fly::DataTypeManager& dataTypeManager = Fly::Internal::GetDataTypeManager();

		const std::string& name = std::visit([&dataTypeManager](const auto& aValue) { return dataTypeManager.GetName(Fly::GetDataTypeID<decltype(aValue)>()); }, aVariant);
		if (ImGui::BeginCombo("##VariantType", name.c_str()))
		{

			ShowSelectableVariantTypes<Types...>(aVariant);
			viewAndEditResult.mIsItemActive = true;

			ImGui::EndCombo();
		}


		
		viewAndEditResult.mIsItemActive |= ViewAndEditVariantType<Types...>(aVariant).mIsItemActive;

		return viewAndEditResult;
	}
}

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

	/*ViewAndEditResult ViewAndEdit(TraitObject& aValue)
	{
		aValue;

		ImGui::Text("%s", Internal::GetTraitByID(aValue.mTraitID).mName.c_str());
		ImGui::Text("%s", Internal::GetNodeType(Internal::GetTraitByID(aValue.mTraitID).mFunctions[aValue.mFunctionIndex].mCallerNodeTypeID).mNodeRecipe.mName.c_str());

		return ViewAndEditResult{};

	}*/

	FLY_VALUETYPE(TraitObject);



	using TestVariant = std::variant<int, float, std::string>;

	FLY_VALUETYPE(TestVariant, CustomName{ "TestVariant" });

	void VariantTest(TestVariant v)
	{
		v;
	}

	FLY_FUNCTION(VariantTest);

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

