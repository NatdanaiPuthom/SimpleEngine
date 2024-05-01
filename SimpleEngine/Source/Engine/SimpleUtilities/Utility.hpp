#pragma once
#include <string>
#include <Windows.h>

namespace SimpleUtilities
{
	static inline std::wstring ToWString(const std::string& aString)
	{
		return std::wstring(aString.begin(), aString.end());
	}

	static inline std::string ConvertTypeIndexNameToPrettyName(const std::string& aNameFromTypeIndex)
	{
		const size_t pos = aNameFromTypeIndex.find_last_of("::"); //NOTE(v9.30.10): I don't know why "::" but it works for now
		
		if (pos != std::string::npos && pos + 1 < aNameFromTypeIndex.length())
		{
			return aNameFromTypeIndex.substr(pos + 1);
		}
	
		return aNameFromTypeIndex;
	}

	static inline const std::string GetAbsolutePath(const char* aFilePath)
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);

		const std::string exePath(buffer);
		const std::string outputPath = exePath.substr(0, exePath.find_last_of("\\/")) + "//" + aFilePath;

		return outputPath;
	}
}