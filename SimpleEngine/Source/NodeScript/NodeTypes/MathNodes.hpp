#pragma once
#include "../FlyDefines.hpp"
#include "../FlyRegistration.hpp"
#include "../Instance/FlyStructInstance.hpp"

namespace FLY_NAMESPACE
{

	ViewAndEditResult Edit(StructInstance& aStructInstance);

	FLY_VALUETYPE(StructInstance);

	StructInstance StructInstanceTest(const StructInstance& aStructInstance);


	FLY_FUNCTION(StructInstanceTest);
}