#pragma once

namespace Math
{
	template<typename T>
	constexpr T Abs(const T& aValue)
	{
		return aValue < 0 ? aValue * -1 : aValue;
	}
}