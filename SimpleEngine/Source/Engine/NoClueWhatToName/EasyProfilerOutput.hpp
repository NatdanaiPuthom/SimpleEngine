#pragma once
#include "Engine/SimpleUtilities/Utility.hpp"
#include "External/profiler.h"

namespace Simple
{
	void EasyProfilerOutput()
	{
		std::string filePath = SimpleUtilities::GetAbsolutePath(SIMPLE_FILENAME_PROFILER);
		filePath;

		const size_t dotPosition = filePath.find_last_of('.');
		const std::string extension = filePath.substr(dotPosition);

#ifdef _DEBUG
		filePath = filePath.substr(0, dotPosition) + "_Debug";
#elif _RELEASE
		filePath = filePath.substr(0, dotPosition) + "_Release";
#elif _SIMPLE
		filePath = filePath.substr(0, dotPosition) + "_Simple";
#endif
		filePath += extension;

		const std::string fileNameWithCounter = SimpleUtilities::AppendCounterIfAlreadyExist(filePath);

		PROFILER_DUMP_FILE(fileNameWithCounter.c_str());
	}
}