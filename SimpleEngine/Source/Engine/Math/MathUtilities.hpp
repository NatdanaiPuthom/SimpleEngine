#pragma once

namespace Math
{
	template<typename T> requires std::is_arithmetic_v<T>
	constexpr T Abs(const T& aValue)
	{
		return aValue < 0 ? -aValue : aValue;
	}
}