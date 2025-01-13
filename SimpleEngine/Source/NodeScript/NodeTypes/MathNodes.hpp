#pragma once
#include "../FlyDefines.hpp"
#include "../FlyRegistration.hpp"
#include "../Instance/FlyStructInstance.hpp"

namespace FLY_NAMESPACE
{

	void TestUtilityFunction();

	ViewAndEditResult ViewAndEdit(StructInstance& aStructInstance);

	StructInstance StructInstanceTest(const StructInstance& aStructInstance);

	FLY_FUNCTION(StructInstanceTest);
}