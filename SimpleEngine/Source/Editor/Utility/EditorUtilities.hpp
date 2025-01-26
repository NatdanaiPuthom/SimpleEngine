#pragma once
#include <concepts>
#include <limits>

namespace Editor
{

	template<std::integral T>
	constexpr T GetInvalidIndex()
	{
		return std::numeric_limits<T>::max();
	}
}