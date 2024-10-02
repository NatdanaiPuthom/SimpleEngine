#pragma once
#include "../FlyDefines.hpp"
#include "../FlyRegistration.hpp"

namespace FLY_NAMESPACE
{

	template<typename To, typename From>
	constexpr To CastValue(From aValue)
	{
		return static_cast<To>(aValue);
	}

	template<typename T>
	constexpr T Clamp(const T& aValue, const T& aMin, const T& aMax)
	{
		return std::clamp(aValue, aMin, aMax);
	}

#undef min
#undef max

	template<typename T>
	constexpr T Min(const T& aValue1, const T& aValue2)
	{
		return std::min(aValue1, aValue2);
	}

	template<typename T>
	constexpr T Max(const T& aValue1, const T& aValue2)
	{
		return std::max(aValue1, aValue2);
	}
}