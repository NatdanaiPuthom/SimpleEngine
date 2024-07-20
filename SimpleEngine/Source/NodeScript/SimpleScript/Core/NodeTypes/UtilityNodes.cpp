#include "UtilityNodes.hpp"
#include "../Node/NodeTypeRegistry.hpp"

namespace FLY_NAMESPACE
{

	template<typename T, typename U>
	static U CastValue(T aValue)
	{
		return static_cast<U>(aValue);
	}

	template<typename T>
	static T Clamp(T aValue, T aMin, T aMax)
	{
		return std::clamp(aValue, aMin, aMax);
	}

#undef min
#undef max

	template<typename T>
	static T Min(T aValue1, T aValue2)
	{
		return std::min(aValue1, aValue2);
	}

	template<typename T>
	static T Max(T aValue1, T aValue2)
	{
		return std::max(aValue1, aValue2);
	}

	REGISTER_FUNCTION(Clamp<int>, "Utility/Function/");
	REGISTER_FUNCTION(Clamp<float>, "Utility/Function/");
	REGISTER_FUNCTION(Clamp<unsigned long long>, "Utility/Function/");

	REGISTER_FUNCTION(Min<int>, "Utility/Function");
	REGISTER_FUNCTION(Min<float>, "Utility/Function");
	REGISTER_FUNCTION(Min<unsigned long long>, "Utility/Function");


	REGISTER_FUNCTION(Max<int>, "Utility/Function");
	REGISTER_FUNCTION(Max<float>, "Utility/Function");
	REGISTER_FUNCTION(Max<unsigned long long>, "Utility/Function");
}

