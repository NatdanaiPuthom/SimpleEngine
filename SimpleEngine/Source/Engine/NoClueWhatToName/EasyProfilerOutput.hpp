#pragma once
#include <fstream>
#include <sstream>
#include <External/profiler.h>
#include "Engine/SimpleUtilities/Utility.hpp"

namespace Simple
{
	void EasyProfilerOutput()
	{
		const std::string baseFileName = SimpleUtilities::GetAbsolutePath(SIMPLE_PROFILER_FILENAME);
		const size_t dotPosition = baseFileName.find_last_of('.');
		const std::string extension = baseFileName.substr(dotPosition);

#ifdef _DEBUG
		const std::string configuration = "_Debug(";
#elif _RELEASE
		const std::string configuration = "_Release(";
#elif _SIMPLE
		const std::string configuration = "_Simple(";
#endif

		std::string fileName = baseFileName;
		unsigned int counter = 1;

		while (std::ifstream(fileName.c_str()))
		{
			std::ostringstream oss;
			oss << baseFileName.substr(0, dotPosition) << configuration << counter << ")" << extension;
			fileName = oss.str();
			counter++;
		}

		PROFILER_DUMP_FILE(fileName.c_str());
	}
}
