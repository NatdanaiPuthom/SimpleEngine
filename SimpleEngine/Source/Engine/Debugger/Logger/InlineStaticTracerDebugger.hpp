#pragma once
#include "Engine/SimpleUtilities/MacroUtility.hpp"
#include <Windows.h>
#include <string>

namespace SimpleTracker
{
	struct InlineLogger
	{
		explicit InlineLogger(const std::string& aString)
		{
			OutputDebugStringA("\n");
			OutputDebugStringA("=========================================================== ");
			OutputDebugStringA(aString.c_str());
			OutputDebugStringA(" ===========================================================\n");
		}
	};
}

#define INLINELOGGER(TYPE, NAME, RETURN) inline static TYPE NAME = []() -> TYPE {SimpleTracker::InlineLogger logger(CONVERT_TO_STRING(NAME)); return RETURN; }();

/* NOTE(v11.3.0)
* #include <vector>
*
* Example 1
* INLINELOGGER(std::vector<int>, example1, std::vector<int>());

* Example 2
* inline static std::vector<int> example2 = []() -> std::vector<int>
* 	{
* 		SimpleTracker::InlineLogger logger("example2");
* 		return std::vector<int>();
* 	}();
* 
* Example 3
* class Example3 : public InlineLogger { Example3("Example3"); }
*/