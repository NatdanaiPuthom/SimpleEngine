#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{

	enum class eBitwiseType
	{
		HasFlag,
		Equals
	};

	template<typename T>
	concept Enumerable = std::is_enum_v<T>;


	template<Enumerable T>
	struct EnumCount;

	template<size_t Count>
	struct EnumCountConstant : std::integral_constant<size_t, Count> {};

	template<Enumerable T>
	constexpr std::underlying_type_t<T> EnumCast(T aValue)
	{
		return static_cast<std::underlying_type_t<T>>(aValue);
	}

	template<Enumerable T>
	constexpr T operator&(const T aValue1, const T aValue2)
	{
		return static_cast<T>(EnumCast(aValue1) & EnumCast(aValue2));
	}

	template<Enumerable T>
	constexpr T operator|(const T aValue1, const T aValue2)
	{
		return static_cast<T>(EnumCast(aValue1) | EnumCast(aValue2));
	}

	template<Enumerable T>
	constexpr T operator^(const T aValue1, const T aValue2)
	{
		return static_cast<T>(EnumCast(aValue1) ^ EnumCast(aValue2));
	}

	template<Enumerable T>
	constexpr void operator|=(T& aValue1, const T aValue2)
	{
		aValue1 = aValue1 | aValue2;
	}

	template<Enumerable T>
	constexpr void operator^=(T& aValue1, const T aValue2)
	{
		aValue1 = aValue1 ^ aValue2;
	}

	template<Enumerable T>
	constexpr bool HasFlag(const T aValue1, const T aValue2)
	{
		return std::underlying_type_t<T>(aValue1 & aValue2) > 0;
	}

	template<Enumerable T>
	constexpr bool HasNotFlag(const T aValue1, const T aValue2)
	{
		return !HasFlag(aValue1, aValue2);
	}
}