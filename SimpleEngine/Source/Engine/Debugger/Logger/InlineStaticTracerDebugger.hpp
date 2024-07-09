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

//#include <vector>
//inline static std::vector<int> example = []() -> std::vector<int>
//	{
//		SimpleTracker::InlineLogger logger("example");
//		return {};
//	}();