#pragma once
#include <string>

namespace SimpleUtilities
{
	inline std::wstring ToWString(const std::string& aString)
	{
		return std::wstring(aString.begin(), aString.end());
	}
}
