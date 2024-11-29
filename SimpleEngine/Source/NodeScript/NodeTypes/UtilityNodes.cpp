#include "UtilityNodes.hpp"

namespace FLY_NAMESPACE
{
	float IntToFloat(int aValue)
	{
		return CastValue<float>(aValue);
	}

	FLY_FUNCTION(IntToFloat, Directory{ "Utility/Cast" }, Pure{});


	FLY_FUNCTION(Clamp<int>, Directory{ "Utility/Function" }, InputNames{ "Value", "Min", "Max" }, DefaultValues{ 0, 0, 0 });
	FLY_FUNCTION(Clamp<float>, Directory{ "Utility/Function" }, InputNames{ "Value", "Min", "Max" }, DefaultValues{ 0.f, 0.f, 0.f });
	FLY_FUNCTION(Clamp<unsigned long long>, Directory{ "Utility/Function" }, InputNames{ "Value", "Min", "Max" }, DefaultValues{ 0ull, 0ull, 0ull });

	FLY_FUNCTION(Min<int>, Directory{ "Utility/Function" });
	FLY_FUNCTION(Min<float>, Directory{ "Utility/Function" });
	FLY_FUNCTION(Min<unsigned long long>, Directory{ "Utility/Function" });


	FLY_FUNCTION(Max<int>, Directory{ "Utility/Function" });
	FLY_FUNCTION(Max<float>, Directory{ "Utility/Function" });
	FLY_FUNCTION(Max<unsigned long long>, Directory{ "Utility/Function" });
	
}

