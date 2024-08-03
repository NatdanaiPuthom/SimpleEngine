#include "UtilityNodes.hpp"

namespace FLY_NAMESPACE
{
	float IntToFloat(int aValue)
	{
		return CastValue<float>(aValue);
	}

	FLY_FUNCTION(IntToFloat, "Utility/Cast/Int To Float");
	

	FLY_FUNCTION(Clamp<int>, "Utility/Function/", InputNames{ "Value", "Min", "Max" }, DefaultValues{ 0, 0, 0 });
	FLY_FUNCTION(Clamp<float>, "Utility/Function/", InputNames{ "Value", "Min", "Max" }, DefaultValues{ 0.f, 0.f, 0.f });
	FLY_FUNCTION(Clamp<unsigned long long>, "Utility/Function/", InputNames{ "Value", "Min", "Max" }, DefaultValues{ 0ull, 0ull, 0ull });

	FLY_FUNCTION(Min<int>, "Utility/Function");
	FLY_FUNCTION(Min<float>, "Utility/Function");
	FLY_FUNCTION(Min<unsigned long long>, "Utility/Function");


	FLY_FUNCTION(Max<int>, "Utility/Function");
	FLY_FUNCTION(Max<float>, "Utility/Function");
	FLY_FUNCTION(Max<unsigned long long>, "Utility/Function");
	
}

