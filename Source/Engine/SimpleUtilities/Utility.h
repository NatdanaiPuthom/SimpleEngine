#pragma once

namespace SimpleUtilities
{
	inline std::wstring ToWString(const std::string& aString)
	{
		return std::wstring(aString.begin(), aString.end());
	}

	template<typename T>
	static inline T Lerp(const T aStart, const T aEnd, const float aPercent)
	{
		return aStart + aPercent * (aEnd - aStart);
	}
}
