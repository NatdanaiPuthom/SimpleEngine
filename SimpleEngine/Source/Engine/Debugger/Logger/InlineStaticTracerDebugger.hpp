#pragma once
#include <Windows.h>
#include <string>

namespace SimpleTracker
{
	struct InlineLogger
	{
		InlineLogger(const std::string& aString)
		{
			OutputDebugStringA("\n");
			OutputDebugStringA("===========================================================");
			OutputDebugStringA(aString.c_str());
			OutputDebugStringA("===========================================================\n");
		}
	};
}

//inline static std::vector<int> example = []
//	{
//		SimpleTracker::InlineLogger logger("example");
//		return std::vector<int>();
//	}();